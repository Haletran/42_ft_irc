#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <csignal> //-> for signal()
#include <cstring>
#include <cstdlib>
#include "Client.hpp"
class Client;
class Server
{
	private :
		int _port;
		std::string _pwd;
		int _server_socket;
		static bool _signal;
		std::vector<Client> _clients;
		std::vector<struct pollfd> _pollfds;
	public :
		Server();
		void ServerInit(int port, std::string pwd);
		void SerSocket();
		void AcceptClient();
		void ReceiveNewData(int fd);
		static void SignalHandler(int signum);
		void CloseFds();
		void ClearClients(int fd);
		std::string GetPwd();
		void SetPwd(std::string pwd);
		Client *getClientByFd(int fd);
		void AuthenticateClient(int fd);
		~Server();
};
#endif