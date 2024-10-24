#pragma once
#include "Global.hpp"

std::string trimNewline(const std::string &str);
class Client;
class Channel;
class Server
{
	protected :
		int _port;
		std::string _pwd;
		int _server_socket;
		static bool _signal;
		std::vector<Client> _clients;
		std::vector<struct pollfd> _pollfds;
		std::map<Channel*, std::vector<Client*> > _channels;
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
		Channel *getChannelByName(const std::string &channel);
		void SendMessageToChannel(const std::string& channel_name, Client* sender, const std::string& message);
		//void AddMessageToChannel(const std::string &channel, const std::string &msg);
		//std::vector<std::string> GetMessagesFromChannel(const std::string &channel); //todo
		std::vector<Client*> GetClientsFromChannel(const std::string &channel); //todo
		void SendPrivateMessage(const std::string &nick, const std::string &msg, Client *client); //todo
		void ProcedeMessage(const std::string &msg, Client *client);
		void ProcedeCommand(const std::string &msg, Client *client);
		void KickFromChannel(const std::string &nick, const std::string &channel, Client *client);
		Client *get_ClientByUsername(std::string username);
		~Server();
};
