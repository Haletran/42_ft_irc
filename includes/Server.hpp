#pragma once
#include "Global.hpp"

class Client;
class Channel;
class Server
{
	protected :
		int _port;
		int _steps;
		std::string _pwd;
		int _server_socket;
		static bool _signal;
		std::vector<Client*> _clients;
		std::vector<struct pollfd> _pollfds;
		std::map<Channel*, std::vector<Client*> > _channels;
		std::map<std::string, void (Server::*)(t_input*)> _commands;
		std::map<std::string, std::string(Server::*)(t_input*, std::string, Channel *channel)> _modeOptions;
	public :
	   Server();
	   ~Server();

	   // Initialization
	   void ServerInit(int port, std::string pwd);
	   void SerSocket();
	   void ReceiveNewData(int fd);

	   // Client Management
	   void AcceptClient();
	   void ClearClients(int fd);
	   Client *getClientByFd(int fd);
	   Client *get_ClientByNickname(std::string username);
	   void AuthenticateClient(int fd, std::string buffer);

	   // Channel Management
	   void CreateChannel(const std::string &channel, Client *client);
	   void JoinChannel(const std::string &channel, Client *client, std::string parameters);
	   void LeaveChannel(const std::string &channel, Client *client);
	   Channel *getChannelByName(const std::string &channel);
	   void KickFromChannel(const std::string &nick, const std::string &channel, Client *client);

	   // Messaging
	   void SendMessageToSomeone(Client *client, std::string msg_content, std::string nickname);
	   void SendInfos(const std::string &channel_name, Client *client);
	   void SendMessageToChannel(const std::string& channel_name, Client* sender, const std::string& message);
	   void SendPrivateMessage(const std::string &nick, const std::string &msg, Client *client); //todo

	   // Command Processing
	   void executeCommand(t_input *input);
	   void ProcedeMessage(const std::string &msg, Client *client);
	   void ProcedeCommand(const std::string &msg, Client *client);

	   // Utility
	   static void SignalHandler(int signum);
	   std::string GetPwd();
	   void SetPwd(std::string pwd);
	   bool ParsingMode(t_input *input, Channel *channel);

		// UTILS
	   void CloseFds();
	   std::vector<Client*> GetClientsFromChannel(const std::string &channel);
	   Client *get_ClientByUsername(std::string username);
	   Channel *getCurrentChannel(Client *client);
	   void CleanServer();
	   void printtabclient_fd(std::vector<Client> _clients);
	   bool isValidUsername(const std::string& username);
	   bool isValidChannelName(const std::string& channel_name);
	   void SendQuittingMessage(Client *client);

	   // COMMAND
	   void JoinCommand(t_input *input);
	   void InviteCommand(t_input *input);
	   void KickCommand(t_input *input);
	   void MsgCommand(t_input *input);
	   void TopicCommand(t_input *input);
	   void ModeCommand(t_input *input);
	   void PartCommand(t_input *input);
	   void WhoCommand(t_input *input);
	   void QuitCommand(t_input *input);


		// MODE
	   std::string OpMode(t_input *input, std::string params, Channel *channel);
	   std::string InviteOnlyMode(t_input *input, std::string params, Channel *channel);
	   std::string TopicMode(t_input *input, std::string params, Channel *channel);
	   std::string KeyMode(t_input *input, std::string params, Channel *channel);
	   std::string LimitMode(t_input *input, std::string params, Channel *channel);
};
