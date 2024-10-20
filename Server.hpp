#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <sstream>
#include <vector> //-> for vector
#include <map> //-> for map
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
		std::map<std::string, std::vector<Client*> > _channels;
		std::map<std::string, std::vector<std::string> > _channelMessages;
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
		void AuthenticateClient(int fd, std::string buffer);
		void CreateChannel(const std::string &channel, Client *client);
		void JoinChannel(const std::string &channel, Client *client);
		void LeaveChannel(const std::string &channel, Client *client);
		void SendMessageToChannel(const std::string &channel, const std::string &msg, Client *client);
		void AddMessageToChannel(const std::string &channel, const std::string &msg);
		std::vector<std::string> GetMessagesFromChannel(const std::string &channel); //todo
		std::vector<Client*> GetClientsFromChannel(const std::string &channel); //todo
		void SendPrivateMessage(const std::string &nick, const std::string &msg, Client *client); //todo
		void ProcedeMessage(const std::string &msg, Client *client);
		void ProcedeCommand(const std::string &msg, Client *client);
		void ProcedeChannelMessage(const std::string &msg, Client *client);
		~Server();
};
#endif