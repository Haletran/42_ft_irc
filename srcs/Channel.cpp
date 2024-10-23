#include "../includes/Global.hpp"

Channel::Channel() : _channel_name("#general"),  _topic("Nothing particular"), _invite_only(false)
{
}

Channel::Channel(std::string channel_name) : _channel_name(channel_name), _topic("Nothing particular"), _invite_only(false)
{
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

