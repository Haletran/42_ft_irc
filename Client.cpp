#include "Client.hpp"

Client::Client()
{
	_fd = -1;
	_auth = false;
}

int Client::GetFd()
{
	return _fd;
}

void Client::SetFd(int fd)
{
	_fd = fd;
}

void Client::SetIp(std::string ip)
{
	_ip = ip;
}

bool Client::GetAuth()
{
	return _auth;
}

void Client::SetAuth(bool auth)
{
	_auth = auth;
}

void Client::SendMsg(const std::string &msg)
{
	if (_fd != -1)
	{
		if (send(_fd, msg.c_str(), msg.length(), 0) == -1)
			std::cerr << "Error sending message" << std::endl;
	}
	else
		std::cerr << "Client fd is -1" << std::endl;
}

Client::~Client()
{
}