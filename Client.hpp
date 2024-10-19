#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "Server.hpp"

class Client
{
	private :
		int _fd;
		std::string _ip;
	public :
		Client();
		int GetFd();
		void SetFd(int fd);
		void SetIp(std::string ip);
};
#endif