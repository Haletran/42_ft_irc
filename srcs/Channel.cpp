#include "../includes/Global.hpp"

Channel::Channel() : _channel_name("#general"),  _topic("Nothing particular"), _invite_only(false), _password(NULL), _user_limit(std::numeric_limits<int>::max())
{
}

Channel::Channel(std::string channel_name)
{
    if (channel_name[0] != '#')
    {
        throw ChannelException(CHANNEL_NAME_ERROR);
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

int Channel::getNbUser() const
{
    return (_clients.size());
}

int Channel::getlimit()
{
    return (_user_limit);

}

void Channel::addOperators(Client *client)
{
    if (!client)
        throw std::runtime_error(OPERATOR_ERROR);
    _operators.push_back(client);
}

void Channel::addInvited(Client* client)
{
    if (!client)
        throw std::runtime_error("Client pointer is null");
    _invited_clients.push_back(client);
}

void Channel::getAllUser(Client *client)
{
    std::string array_op;
    std::string array_user;
    _clients.push_back(client);
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        array_op += "@" + (*it)->getNickname() + " ";
    }
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        array_user += (*it)->getNickname() + " ";
    }
    client->SendMsg(LIST_USER);
}


bool Channel::IsOP(Client *client)
{
    std::string username = client->GetUsername();
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if ((*it)->GetUsername() == username)
        {
            return true;
        }
    }
    return false;
}

bool Channel::IsInvited(Client*client)
{
    std::string username = client->GetUsername();
    for (std::vector<Client*>::iterator it = _invited_clients.begin(); it != _invited_clients.end(); ++it)
    {
        if ((*it)->GetUsername() == username)
            return true;
    }
    return false;
}

Channel::~Channel()
{
}
