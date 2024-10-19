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
