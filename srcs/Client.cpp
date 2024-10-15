#include "../includes/Client.hpp"

Client::Client(int state) : _state(state)
{
}

Client::~Client() {}


Client::Client(const Client& cpy)
{
    *this = cpy;
}

Client &Client::operator=(const Client &src)
{
    if (this != &src)
    {
        this->_state = src._state;
        this->_username = src._username;
        this->_nickname = src._nickname;
    }
    return (*this);
}

void Client::set_username(std::string username)
{
    this->_username = username;
}

void Client::set_state(int state)
{
    this->_state = state;
}

void Client::set_nickname(std::string nickname)
{
    this->_nickname = nickname;
}

int Client::get_state()
{
    return (_state);
}

std::string Client::get_username()
{
    return (_username);
}

std::string Client::get_nickname()
{
    return (_nickname);
}
