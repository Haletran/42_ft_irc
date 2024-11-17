#pragma once
#include "Global.hpp"

class Client
{
	private :
		int _fd;
		std::string _ip;
		bool _auth;
		std::string _nick;
		std::string _username;
	public :
	   // Constructors
	   Client();
	   Client(Client const &src);

	   // Destructor
	   ~Client();

	   // Getters
	   int GetFd();
	   std::string GetNick();
	   std::string GetUsername();
	   bool GetAuth();

	   // Setters
	   void SetFd(int fd);
	   void SetIp(std::string ip);
	   void SetNick(std::string nick);
	   void SetUsername(std::string username);
	   void SetAuth(bool auth);

	   // Other member functions
	   void SendMsg(const std::string &msg);
	   std::string getNickname();
};
