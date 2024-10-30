#include "../includes/Global.hpp"

bool Server::_signal = false;

Server::Server()
{
	_server_socket = -1;
}

Server::~Server()
{
}

void Server::ClearClients(int fd)
{
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].fd == fd)
		{
			_pollfds.erase(_pollfds.begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].GetFd() == fd)
		{
			_clients.erase(_clients.begin() + i);
			break;
		}
	}
}

void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << "Signal received" << std::endl;
	_signal = true;
}

void Server::CloseFds()
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << "Closing fd: " << _clients[i].GetFd() << std::endl;
		close(_clients[i].GetFd());
	}
	if (_server_socket != -1)
	{
		std::cout << "Closing server socket" << std::endl;
		close(_server_socket);
	}
}

void Server::SerSocket()
{
	struct sockaddr_in server_addr;
	struct pollfd newpoll;
	memset(&newpoll, 0, sizeof(newpoll));
	server_addr.sin_family = AF_INET; // IPv4
	server_addr.sin_addr.s_addr = INADDR_ANY; // Any local IP
	server_addr.sin_port = htons(_port); // convert Port

	_server_socket = socket(AF_INET, SOCK_STREAM, 0); //set socket and returns fd
	if (_server_socket == -1)
		throw std::runtime_error("Failed to create socket");
	int opt = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) // Reuse address and port
		throw std::runtime_error("Failed to set socket options");
	if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) == -1) // Set non-blocking
		throw std::runtime_error("Failed to set non-blocking");
	if (bind(_server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) // Bind socket
		throw std::runtime_error("Failed to bind socket");
	if (listen(_server_socket, SOMAXCONN) == -1) // Listen for connections and making passive socket
		throw std::runtime_error("Failed to listen for connections");
	newpoll.fd = _server_socket;
	newpoll.events = POLLIN;
	newpoll.revents = 0;
	_pollfds.push_back(newpoll);
}

void Server::ServerInit(int port, std::string pwd)
{
	if (port < 1024 || port > 65535)
		throw std::runtime_error("Invalid port number");
	_port = port;
	if (pwd.empty())
		throw std::runtime_error("Password cannot be empty");
	_pwd = pwd;
	SerSocket();
	std::cout << "Server started on port: " << _port << std::endl;
	std::cout << "Waiting for connections..." << std::endl;

	while (!_signal)
	{
		if ((poll(&_pollfds[0], _pollfds.size(), -1) == -1) && _signal == false)
			throw std::runtime_error("Failed to poll");
		for (size_t i = 0; i < _pollfds.size(); i++) // check all fds for event, -1 means block until event
		{
			if (_pollfds[i].revents & POLLIN) //check for data to read
			{
				if (_pollfds[i].fd == _server_socket) // check if server socket
					AcceptClient();
				else
					ReceiveNewData(_pollfds[i].fd);
			}
		}
	}
	CloseFds();
}

std::vector<std::string> splitString(const std::string &str) {
    std::istringstream stream(str);
    std::string word;
    std::vector<std::string> result;
    while (stream >> word) {
        result.push_back(word);
    }
    return result;
}

void Server::AuthenticateClient(int fd, std::string buffer)
{
    Client *client = getClientByFd(fd);
	if (client == NULL)
		return;
	std::istringstream stream(buffer);
	std::string line;
	while (std::getline(stream, line))
	{
		if (line.find("PASS ") == 0)
		{
			std::string pwd = line.substr(5);
			pwd = trimNewline(pwd);
			if (pwd == _pwd)
			{
				client->SetAuth(true);
				std::string msg = "Authenticated\n";
				send(fd, msg.c_str(), msg.length(), 0);
				std::cout << "Client authenticated: " << fd << std::endl;
			}
			else
			{
				std::string msg = "Authentication failed\n";
				send(fd, msg.c_str(), msg.length(), 0);
				ClearClients(fd);
				close(fd);
			}
		}
		else if (line.find("NICK ") == 0)
		{
			std::string nick = line.substr(5);
			nick = trimNewline(nick);
			client->SetNick(nick);
			std::string msg = "Nick set to " + nick + "\n";
			send(fd, msg.c_str(), msg.length(), 0);
		}
		else if (line.find("USER ") == 0)
		{
            std::vector<std::string> parts = splitString(line);
            if (parts.size() >= 2)
			{
                std::string username = parts[1];
                username = trimNewline(username);
                client->SetUsername(username);
                std::string msg = "Username set to " + username + "\n";
                send(fd, msg.c_str(), msg.length(), 0);
			}
		}
	}
}

void Server::AcceptClient()
{
	Client new_client;
	struct sockaddr_in client_addr;
	struct pollfd newpoll;
	socklen_t addr_size = sizeof(client_addr);
	//int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen); *addr = client address(retreive client ip and port) accept() returns client socket

	memset(&newpoll, 0, sizeof(newpoll));
	int new_fd = accept(_server_socket, (struct sockaddr *)&client_addr, &addr_size); // accept connection
	if (new_fd == -1)
		{std::cerr << "Failed to accept connection" << std::endl;return;}
	if (fcntl(new_fd, F_SETFL, O_NONBLOCK) == -1) // set non-blocking
		{std::cerr << "Failed to set non-blocking" << std::endl; close(new_fd); return;}
	newpoll.fd = new_fd;
	newpoll.events = POLLIN;
	newpoll.revents = 0;
	_pollfds.push_back(newpoll);

	new_client.SetFd(new_fd);
	new_client.SetIp(inet_ntoa(client_addr.sin_addr)); // convert ip to string
	_clients.push_back(new_client);
	std::cout << "New connection from: " << new_fd << std::endl;
}


void Server::ReceiveNewData(int fd)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0); // read data from client
	if (bytes_read <= 0)
	{
		std::cout << "Client disconnected: " << fd << std::endl;
		ClearClients(fd);
		close(fd);
		return;
	}
	else
	{
		buffer[bytes_read] = '\0';
		std::cout << fd << " : " << buffer <<std::endl;
	}
	if (getClientByFd(fd)->GetAuth() == false || getClientByFd(fd)->GetNick().empty() || getClientByFd(fd)->GetUsername().empty())
		AuthenticateClient(fd, buffer);
	else
		ProcedeMessage(buffer, getClientByFd(fd));
}

Client *Server::getClientByFd(int fd)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].GetFd() == fd)
			return &_clients[i];
	}
	return NULL;
}

void Server::JoinChannel(const std::string &channel_name, Client *client)
{
    if (!client->GetAuth() || client->GetNick().empty() || client->GetUsername().empty())
    {
        std::cerr << "Client is not fully authenticated: " << client->GetFd() << std::endl;
        return;
    }
    std::string trimmed_channel_name = trimNewline(channel_name);
    Channel* channel = getChannelByName(trimmed_channel_name);
    std::string joinMsg = ":" + client->GetNick() + "!" + client->GetUsername() + "@localhost JOIN :" + trimmed_channel_name + "\r\n";
    if (channel != NULL)
    {
        if (channel->getInvite() == true)
        {
            std::string error_msg = ":localhost 473 " + client->GetNick() + " " + trimmed_channel_name + " :Cannot join channel (+i) - you must be invited\r\n";
            client->SendMsg(error_msg);
            return;
        }
        _channels[channel].push_back(client);
        for (std::vector<Client*>::iterator it = _channels[channel].begin(); it != _channels[channel].end(); ++it)
            (*it)->SendMsg(joinMsg);
    }
    else
    {
        try
        {
            Channel* newChannel = new Channel(trimmed_channel_name);
            _channels[newChannel].push_back(client);
            client->SendMsg(joinMsg);
        }
        catch (Channel::ChannelException &e)
        {
            std::cerr << e.what() << std::endl;
            std::string errMsg = "Error: " + std::string(e.what()) + "\r\n";
            client->SendMsg(errMsg);
            return;
        }
    }
}


void Server::LeaveChannel(const std::string &channel_name, Client *client)
{
    Channel* channel = getChannelByName(channel_name);
    if (channel != NULL)
    {
        std::vector<Client*>& clients = _channels[channel];
        for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
        {
            if (*it == client)
            {
                clients.erase(it);
                std::string msg = "Client left channel " + channel_name + "\n";
                send(client->GetFd(), msg.c_str(), msg.length(), 0);
                break;
            }
        }
    }
    else
    {
        std::string msg = "Channel does not exist\n";
        send(client->GetFd(), msg.c_str(), msg.length(), 0);
    }
}

Channel* Server::getChannelByName(const std::string &channel) {
    std::map<Channel*, std::vector<Client*> >::iterator it;
    for (it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->first->GetChannelName() == channel) {
            return it->first;
        }
    }
    return NULL;
}

void printallChannel(std::map<std::string, std::vector<Client*> > _channels)
{
	std::map<std::string, std::vector<Client*> >::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++)
	{
		std::cout << "Channel: " << it->first << std::endl;
		std::vector<Client*>::iterator clientIt;
		for (clientIt = it->second.begin(); clientIt != it->second.end(); clientIt++)
		{
			std::cout << "Client: " << (*clientIt)->GetUsername() << std::endl;
		}
	}
}

std::vector<Client*> Server::GetClientsFromChannel(const std::string &channel)
{
    Channel* channelPtr = getChannelByName(channel);
    if (channelPtr != NULL) {
        return _channels[channelPtr];
    } else {
        std::cerr << "Channel does not exist" << std::endl;
        return std::vector<Client*>();
    }
}

Client *Server::get_ClientByUsername(std::string username)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].GetUsername() == username)
            return &_clients[i];
    }
    return NULL;
}
