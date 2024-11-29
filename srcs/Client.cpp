#include "../includes/Global.hpp"

Client::Client()
{
	_fd = -1;
	_auth = false;
}

Client::Client(Client const &src)
{
	*this = src;
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
    if (_fd == -1)
    {
        std::cerr << "Client fd is -1" << std::endl;
        return;
    }
    ssize_t bytes_sent = send(_fd, msg.c_str(), msg.length(), 0);
    if (bytes_sent == -1)
    {
        std::cerr << _fd <<": Error sending message: " << strerror(errno) << " (errno: " << errno << ")\n";
    }
    else if (bytes_sent < static_cast<ssize_t>(msg.length()))
    {
        std::cerr << "Partial message sent: " << bytes_sent << " out of " << msg.length() << " bytes\n";
    }
    else{
        std::cerr << SEND_DEBUG << std::endl;
    }
}

void Client::SetNick(std::string nick)
{
	_nick = nick;
}

std::string Client::GetNick()
{
	return _nick;
}

void Client::SetUsername(std::string username)
{
	_username = username;
}

std::string Client::GetUsername()
{
	return _username;
}

std::string Client::getNickname()
{
	return _nick;
}

std::string Client::GetBuffer()
{
    return _buffer;
}

void Client::SetBuffer(std::string buffer)
{
    _buffer = buffer;
}

Client::~Client()
{
}
