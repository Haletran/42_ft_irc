#include "Server.hpp"

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
	/* struct sockaddr_in {
	sa_family_t     sin_family;
	in_port_t       sin_port;
	struct  in_addr sin_addr;
	char            sin_zero[8];
	}; */
	
	
	/* struct in_addr {
 	in_addr_t s_addr;
	}; */

	/* struct pollfd {
	int     fd; //-> file descriptor
	short   events;//-> requested events
	short   revents;//-> returned events
	}; */


	struct sockaddr_in server_addr;
	struct pollfd newpoll;
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

	//int poll(struct pollfd *fds, nfds_t nfds, int timeout);
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

std::string trimNewline(const std::string &str) {
    size_t end = str.find_last_not_of("\r\n ");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
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
	int new_fd = accept(_server_socket, (struct sockaddr *)&client_addr, &addr_size); // accept connection
	if (new_fd == -1)
		{std::cerr << "Failed to accept connection" << std::endl;return;}
	if (fcntl(new_fd, F_SETFL, O_NONBLOCK) == -1) // set non-blocking
		{std::cerr << "Failed to set non-blocking" << std::endl;return;}
	newpoll.fd = new_fd;
	newpoll.events = POLLIN;
	newpoll.revents = 0;
	_pollfds.push_back(newpoll);

	new_client.SetFd(new_fd);
	new_client.SetIp(inet_ntoa(client_addr.sin_addr)); // convert ip to string
	_clients.push_back(new_client);
	std::cout << "New connection from: " << new_fd << std::endl;
	
}

void Server::ProcedeCommand(const std::string &msg, Client *client)
{
	if (msg.find("JOIN") != std::string::npos)
	{
		std::string channel = msg.substr(5);
		if (channel[channel.length() - 1] == '\n')
			channel = channel.substr(0, channel.length() - 1);
		JoinChannel(channel, client);
	}
	else if (msg.find("LEAVE") != std::string::npos)
	{
		std::string channel = msg.substr(6);
		if (channel[channel.length() - 1] == '\n')
			channel = channel.substr(0, channel.length() - 1);
		LeaveChannel(channel, client);
	}
	else if (msg.find("WHOAMI") != std::string::npos)
	{
		std::string msg = "Nick: " + client->GetNick() + "\n";
		client->SendMsg(msg);
		msg = "Username: " + client->GetUsername() + "\n";
		client->SendMsg(msg);
	}
	else
	{
		std::string msg = "Unknown command\n";
		client->SendMsg(msg);
	}
}

void Server::ProcedeChannelMessage(const std::string &msg, Client *client)
{
    std::string prefix = "PRIVMSG ";
	if (msg.find(prefix) != 0)
		return;
	std::istringstream stream(msg.substr(prefix.length()));
	std::string channel;
	std::string message;
	std::string separator;

	stream >> channel >> separator;
	std::getline(stream, message);
	if (!message.empty() && message[0] == ' ')
		message = message.substr(1);
	channel = trimNewline(channel);
	message = trimNewline(message);
	std::string formatted_msg = ":" + client->GetUsername() + " PRIVMSG " + channel + " " + message + "\r\n";
    SendMessageToChannel(channel, formatted_msg, client);
    //AddMessageToChannel(channel, formatted_msg);
}

void Server::ProcedeMessage(const std::string &msg, Client *client)
{
	std::string channel;
	std::string message;
	std::istringstream stream(msg);
	std::string line;
	while (std::getline(stream, line))
	{
		if (line.find("PRIVMSG ") == std::string::npos)
			ProcedeCommand(line, client);
		else
			ProcedeChannelMessage(line, client);
	}
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

	std::string join_msg = ":" + client->GetUsername() + " JOIN " + trimmed_channel_name + "\r\n";
	client->SendMsg(join_msg);

	if (_channels.find(trimmed_channel_name) != _channels.end())
	{
		_channels[trimmed_channel_name].push_back(client);
		std::string msg = ":" + client->GetUsername() + " has joined the channel " + trimmed_channel_name + "\r\n";
		std::vector<Client*> clientsInChannel = GetClientsFromChannel(trimmed_channel_name);
		for (std::vector<Client*>::iterator clientIt = clientsInChannel.begin(); clientIt != clientsInChannel.end(); ++clientIt)
		{
			if (*clientIt != client)
			{
				(*clientIt)->SendMsg(msg);
			}
		}
	}
	else
	{
		_channels[trimmed_channel_name] = std::vector<Client*>();
		_channels[trimmed_channel_name].push_back(client);
		std::string msg = "Channel " + trimmed_channel_name + " created\r\n";
		client->SendMsg(msg);
		msg = "You joined channel " + trimmed_channel_name + "\r\n";
		client->SendMsg(msg);
	}
}

void Server::LeaveChannel(const std::string &channel_name, Client *client)
{
    if (_channels.find(channel_name) != _channels.end())
    {
        for (size_t i = 0; i < _channels[channel_name].size(); i++)
        {
            if (_channels[channel_name][i] == client)
            {
                _channels[channel_name].erase(_channels[channel_name].begin() + i);
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

void Server::SendMessageToChannel(const std::string &channelName, const std::string &msg, Client *client)
{
    std::string trimmedChannelName = trimNewline(channelName);

    // Cherche le channel dans la liste des channels existants
    std::map<std::string, std::vector<Client*> >::iterator it = _channels.find(trimmedChannelName);
    if (it != _channels.end())
    {
        // Formate le message sans l'IP de l'expéditeur
        std::string formatted_msg = ":" + client->GetUsername() + "!" + client->GetNick() + " PRIVMSG " + trimmedChannelName + " :" + msg + "\r\n";

        // Parcourt tous les clients dans le channel
        std::vector<Client*>::iterator clientIt;
        for (clientIt = it->second.begin(); clientIt != it->second.end(); ++clientIt)
        {
            // Envoie le message à tous les autres clients sauf celui qui l'a envoyé
            if (*clientIt != client)
            {
                (*clientIt)->SendMsg(formatted_msg);  // Envoie le message formaté au client
            }
        }
    }
    else 
    {
        // Si le channel n'existe pas, envoie un message d'erreur au client expéditeur
        std::string error_msg = "Channel does not exist\n";
        client->SendMsg(error_msg);
    }
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

void Server::AddMessageToChannel(const std::string &channel, const std::string &msg) {
    //printallChannel(_channels);
	std::map<std::string, std::vector<Client*> >::iterator it;
	
	if (_channels.find(channel) != _channels.end()) {
		
        _channelMessages[channel].push_back(msg);
    } else {
        std::cerr << "Channel "<< channel <<" does not exist" << std::endl;
    }
	
}

std::vector<std::string> Server::GetMessagesFromChannel(const std::string &channel) {
    if (_channels.find(channel) != _channels.end()) {
        return _channelMessages[channel];
    } else {
        std::cerr << "Channel does not exist" << std::endl;
        return std::vector<std::string>();
    }
}

std::vector<Client*> Server::GetClientsFromChannel(const std::string &channel) {
	if (_channels.find(channel) != _channels.end()) {
		return _channels[channel];
	} else {
		std::cerr << "Channel does not exist" << std::endl;
		return std::vector<Client*>();
	}
}