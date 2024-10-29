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

const std::string &Channel::GetChannelName()
{
	return _channel_name;
}

bool Channel::operator==(const std::string &other) const
{
        return _channel_name == other;
}

Channel::~Channel()
{
}
