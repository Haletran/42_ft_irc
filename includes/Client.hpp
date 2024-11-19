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
		std::string _buffer;
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
	   std::string GetBuffer();

	   // Setters
	   void SetFd(int fd);
	   void SetIp(std::string ip);
	   void SetNick(std::string nick);
	   void SetUsername(std::string username);
	   void SetAuth(bool auth);
	   void SetBuffer(std::string buffer);

	   // Other member functions
	   void SendMsg(const std::string &msg);
	   std::string getNickname();
};
