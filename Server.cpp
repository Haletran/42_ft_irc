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

void Server::AuthenticateClient(int fd)
{
    Client *client = getClientByFd(fd);
	if (client == NULL)
		return;
	std::string msg = "Use PASS <password> to authenticate\n";
	send(fd, msg.c_str(), msg.length(), 0);
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
	AuthenticateClient(new_fd);
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
		std::cout << "Received data from: " << fd << "Data :" << buffer <<std::endl;
	}
	if (getClientByFd(fd)->GetAuth() == false)
	{
		if (strncmp(buffer, "PASS ", 5) == 0)
		{
			std::string pwd = buffer + 5;
			if (pwd[pwd.length() - 1] == '\n')
				pwd = pwd.substr(0, pwd.length() - 1);
			if (pwd == _pwd)
			{
				getClientByFd(fd)->SetAuth(true);
				std::string msg = "Authenticated\n";
				send(fd, msg.c_str(), msg.length(), 0);
				std::cout << "Client authenticated: " << fd << std::endl;
			}
			else
			{
				std::string msg = "Authentication failed\n";
				send(fd, msg.c_str(), msg.length(), 0);
			}
		}
		else
		{
			std::string msg = "Use PASS <password> to authenticate\n";
			send(fd, msg.c_str(), msg.length(), 0);
		}
	}
	else
	{
		std::string msg = "Unknown command\n";
		send(fd, msg.c_str(), msg.length(), 0);
	}
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