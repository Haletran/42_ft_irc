#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "Server.hpp"

class Client
{
	private :
		int _fd;
		std::string _ip;
		bool _auth;
	public :
		Client();
		int GetFd();
		void SetFd(int fd);
		void SetIp(std::string ip);
		bool GetAuth();
		void SetAuth(bool auth);
		void SendMsg(const std::string &msg);
		~Client();

};
#endif