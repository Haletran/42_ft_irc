#include "../includes/Global.hpp"

Channel::Channel() : _channel_name("#general"),  _topic("Nothing particular"), _invite_only(false), _password(NULL), _user_limit(std::numeric_limits<int>::max())
{
}

Channel::Channel(std::string channel_name)
{
    if (channel_name[0] != '#')
    {
        std::string error_message = ":localhost 403 * " + channel_name + " :Channel name must start with '#'";
        throw ChannelException(error_message);
    }
    else
    {
        _channel_name = channel_name;
        _topic = "Nothing particular";
        _invite_only = false;
		_password = "";
		password_needed = false;
		_user_limit = std::numeric_limits<int>::max();
    }
}

std::string Channel::getTopic()
{
	return _topic;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void Channel::setInviteOnly(bool invite_only)
{
    std::cout << "asfhsdg" << std::endl;
    _invite_only = invite_only;
}

void  Channel::setPassword(std::string password)
{
    _password = password;
}

void Channel::setUserLimit(int user_limit)
{
    _user_limit = user_limit;
}


void Channel::setPasswordNeeded(bool condition)
{
    password_needed = condition;
}

const std::string &Channel::GetChannelName()
{
	return _channel_name;
}

bool Channel::getInvite()
{
    return(_invite_only);
}

bool Channel::operator==(const std::string &other) const
{
        return _channel_name == other;
}

std::string Channel::getFlag()
{
    std::string backup;
    int i = 0;
    if (getInvite() == true)
        backup[i++] = 'i';
    if (!_password.empty())
        backup[i++] = 'k';
    if (_user_limit != std::numeric_limits<int>::max())
        backup[i++] = 'l';
    return (backup);
}

std::string Channel::getPassword()
{
    return (_password);
}

bool Channel::getPasswordNeeded()
{
    return(password_needed);
}


Channel::~Channel()
{
}
