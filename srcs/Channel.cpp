#include "../includes/Global.hpp"

Channel::Channel() : _channel_name("#general"),  _topic("Nothing particular"), _invite_only(false)
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

