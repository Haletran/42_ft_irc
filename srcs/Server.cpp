#include "../includes/Global.hpp"

bool Server::_signal = false;

Server::Server()
{
	_server_socket = -1;
}

Server::~Server()
{
    CleanServer();
}

void Server::CleanServer()
{
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete *it;
    }
    _clients.clear();
    std::map<Channel*, std::vector<Client*> >::iterator it;
    for (it = _channels.begin(); it != _channels.end(); ++it) {
        it->second.clear();
        delete it->first;
    }
    _channels.clear();
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
		if (_clients[i]->GetFd() == fd)
		{
			close(fd);
			delete _clients[i];
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
		std::cout << "Closing fd: " << _clients[i]->GetFd() << std::endl;
		close(_clients[i]->GetFd());
	}
	if (_server_socket != -1)
	{
		close(_server_socket);
	}
}

void Server::SerSocket()
{
	struct sockaddr_in server_addr;
	struct pollfd newpoll;
	memset(&newpoll, 0, sizeof(newpoll));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(_port);

	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1)
		throw std::runtime_error("Failed to create socket");
	int opt = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
		throw std::runtime_error("Failed to set socket options");
	if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking");
	if (bind(_server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		throw std::runtime_error("Failed to bind socket");
	if (listen(_server_socket, SOMAXCONN) == -1)
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
	std::cout << "\033[1;32mPORT : \033[0m" << _port << std::endl;
	std::cout << "\033[1;32mPASSWORD : \033[0m" << _pwd << std::endl;
	std::cout << "\033[1;33mWaiting for connections...\033[0m" << std::endl;
	std::cout << "\033[1;34m----------------\033[0m" << std::endl;

	while (!_signal)
	{
		if ((poll(&_pollfds[0], _pollfds.size(), -1) == -1) && _signal == false)
			throw std::runtime_error("Failed to poll");
		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents & POLLIN)
			{
				if (_pollfds[i].fd == _server_socket)
					AcceptClient();
				else
					ReceiveNewData(_pollfds[i].fd);
			}
		}
	}
	CloseFds();
}

bool Server::isValidUsername(const std::string& username)
{
    if (username.empty() || username.length() > 9)
        return false;
    if (username[0] == '$' || username[0] == ':')
        return false;
    for (size_t i = 0; i < username.length(); i++) {
        char c = username[i];
        if (c == ' ' || c == '\0' || c == '\r' || c == '\n' || c == '@')
            return false;
    }
    return true;
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
				std::cout << "[Client FD: " << fd << "] : Authentication successful" << std::endl;
			}
			else
			{
				std::string msg = "Authentication failed\n";
				send(fd, msg.c_str(), msg.length(), 0);
			}
		}
		if (line.find("NICK ") == 0)
		{
			std::string nick = line.substr(5);
			nick = trimNewline(nick);
			if (get_ClientByNickname(nick) != NULL)
			{
				std::string errorMsg = ":localhost  433 * " + nick + " :Nickname is already in use\r\n";
				send(fd, errorMsg.c_str(), errorMsg.length(), 0);
			}
			else if (check_invalid_chars(nick))
			{
				client->SetNick(nick);
				std::string msg = ":localhost 001 " + nick + " :Welcome to the IRC server\r\n";
				send(fd, msg.c_str(), msg.length(), 0);
			}
			else
			{
				std::string errorMsg = ":localhost 432 * " + nick + " :Erroneous nickname\r\n";
				send(fd, errorMsg.c_str(), errorMsg.length(), 0);
			}
		}
		if (line.find("USER ") == 0)
		{
            std::vector<std::string> parts = splitString(line);
            if (parts.size() >= 2)
			{
                std::string username = parts[1];
                username = trimNewline(username);
				if (!isValidUsername(username))
				{
					std::string errorMsg = ":localhost 432 * " + username + " :Erroneous username\r\n";
           			send(fd, errorMsg.c_str(), errorMsg.length(), 0);
            		return;
				}
				else if (get_ClientByUsername(username) == NULL)
				{
					client->SetUsername(username);
				}
                else
				{
					int i = 1;
                    std::string user_copy;
                    while (1)
                    {
                        std::stringstream ss;
                		ss << username << i;
                		user_copy = ss.str();
                        if (get_ClientByUsername(user_copy) == NULL)
                        {
                            client->SetUsername(user_copy);
                            username = user_copy;
                            break;
                        }
                        i++;
                    }
				}
                std::string msg = "Username set to " + username + "\n";
                send(fd, msg.c_str(), msg.length(), 0);
			}
		}
	}
}



void Server::AcceptClient()
{
	Client * new_client = new Client();
	struct sockaddr_in client_addr;
	struct pollfd newpoll;
	socklen_t addr_size = sizeof(client_addr);

	bzero(&client_addr, sizeof(client_addr));
	memset(&newpoll, 0, sizeof(newpoll));
	int new_fd = accept(_server_socket, (struct sockaddr *)&client_addr, &addr_size);
	if (new_fd == -1)
		{std::cerr << "Failed to accept connection" << std::endl;return;}
	if (fcntl(new_fd, F_SETFL, O_NONBLOCK) == -1)
		{std::cerr << "Failed to set non-blocking" << std::endl; close(new_fd); return;}
	newpoll.fd = new_fd;
	newpoll.events = POLLIN;
	newpoll.revents = 0;
	_pollfds.push_back(newpoll);

	new_client->SetFd(new_fd);
	new_client->SetIp(inet_ntoa(client_addr.sin_addr)); // convert ip to string
	_clients.push_back(new_client);
	std::cout << "-> New connection from: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << " (fd: " << new_fd << ")" << std::endl;
}

void Server::printtabclient_fd(std::vector<Client> _clients)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << "Client fd : " << _clients[i].GetFd() << std::endl;
	}
}

void Server::ReceiveNewData(int fd)
{
    char buffer[20000]; // change to 10000 for bonus
    memset(buffer, 0, sizeof(buffer));
    int bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read < 0)
    {
        std::cout << "Client error on fd: " << fd << std::endl;
        ClearClients(fd);
        close(fd);
        return;
    }

    Client* client = getClientByFd(fd);
    if (!client)
        return;

    if (bytes_read == 0)
    {
        std::string stored_buffer = client->GetBuffer();
        if (!stored_buffer.empty() && stored_buffer.find("\n") != std::string::npos)
        {
            if (client->GetAuth() == false || client->GetNick().empty() || client->GetUsername().empty())
                AuthenticateClient(fd, stored_buffer);
            else
                ProcedeMessage(stored_buffer, client);
            client->SetBuffer("");
        }
        std::cout << "Client disconnected on fd: " << fd << std::endl;
        SendQuittingMessage(client);
        ClearClients(fd);
        //close(fd);
        return;
    }

    buffer[bytes_read] = '\0';
    std::string current_data = buffer;
    std::string complete_data = client->GetBuffer() + current_data;
    if (complete_data.find("\n") == std::string::npos)
    {
        client->SetBuffer(complete_data);
        return;
    }
    client->SetBuffer("");
    if (client->GetAuth() == false || client->GetNick().empty() || client->GetUsername().empty())
        AuthenticateClient(fd, complete_data);
    else
        ProcedeMessage(complete_data, client);
}

Client *Server::getClientByFd(int fd)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->GetFd() == fd)
			return _clients[i];
	}
	return NULL;
}

void Server::SendInfos(const std::string &channel_name, Client *client)
{
    std::string trimmed_channel_name = trimNewline(channel_name);
    Channel* channel = getChannelByName(trimmed_channel_name);
    if (!channel->getTopic().empty() )
        client->SendMsg(GET_TOPIC);
    client->SendMsg(MODE_JOIN);
    client->SendMsg(FLAG_MSG);
    client->SendMsg(CREATION_TIME);
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
                channel->removeClient(*it);
                channel->removeOperator(*it);
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
    if (channel.empty())
        return NULL;
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
        if (_clients[i]->GetUsername() == username)
            return _clients[i];
    }
    return NULL;
}


Client *Server::get_ClientByNickname(std::string username)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i]->GetNick() == username)
            return _clients[i];
    }
    return NULL;
}

Channel *Server::getCurrentChannel(Client *client){
    if (!client)
        return (NULL);
    for (std::map<Channel*, std::vector<Client*> >::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        std::vector<Client*> clients = it->second;
        for (size_t i = 0; i < clients.size(); i++) {
            if (clients[i] == client) {
                return it->first;
            }
        }
    }
    return NULL;
}
