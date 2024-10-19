#include "Client.hpp"

Client::Client()
{
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
