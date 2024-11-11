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
		Client();
		Client(Client const &src);
		int GetFd();
		void SetFd(int fd);
		void SetIp(std::string ip);
		void SetNick(std::string nick);
		std::string GetNick();
		void SetUsername(std::string username);
		std::string GetUsername();
		bool GetAuth();
		void SetAuth(bool auth);
		void SendMsg(const std::string &msg);
		std::string getNickname();

		~Client();
};
