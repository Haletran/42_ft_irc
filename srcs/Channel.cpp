#include "../includes/Global.hpp"

Channel::Channel() : _channel_name("#general"),  _topic("Nothing particular"), _invite_only(false), _password(NULL), _user_limit(std::numeric_limits<int>::max())
{
    this->_topic_change = true;
    std::stringstream ss;
    std::time_t current_time = std::time(0);
    ss << current_time;
    this->time_created = ss.str();
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
		_topic_change = true;
		_user_limit = std::numeric_limits<int>::max();
        std::stringstream ss;
        std::time_t current_time = std::time(0);
        ss << current_time;
        this->time_created = ss.str();
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


void Channel::setTopicChange(bool value)
{
    _topic_change = value;
}

bool Channel::getTopicChange()
{
    return (_topic_change);
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

std::string Channel::getTimeCreated()
{
    return (time_created);
}

void Channel::removeClient(Client *client)
{
    if (!client)
        throw std::runtime_error("Not found in the channel");
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if ((*it)->GetUsername() == client->GetUsername())
        {
            _clients.erase(it);
            return;
        }
    }
}


void Channel::removeOperator(Client *client)
{
    if (!client)
        throw std::runtime_error(OPERATOR_ERROR);
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if ((*it)->GetUsername() == client->GetUsername())
        {
            _operators.erase(it);
            return;
        }
    }
}

bool Channel::operator==(const std::string &other) const
{
        return _channel_name == other;
}

std::string Channel::getFlag()
{
    std::string backup;
    backup += 'C';
    backup += 'n';
    backup += 's';
    if (getInvite() == true)
        backup += 'i';
    if (!_password.empty())
        backup += 'k';
    if (_user_limit != std::numeric_limits<int>::max())
        backup += 'l';
    if (_topic_change == false)
        backup += 't';
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

bool Channel::isAlreadyConnected(Client *client)
{
    std::string username = client->GetNick();
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if ((*it)->GetNick() == username)
            return true;
    }
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if ((*it)->GetNick() == username)
            return true;
    }
    return false;
}

void Channel::CleanChannel(Channel *chan)
{
    clearVector(chan->_operators);
    clearVector(chan->_clients);
    clearVector(chan->_invited_clients);
}


Channel::~Channel()
{
    CleanChannel(this);
    std::cerr << "Channel " << _channel_name << " has been deleted" << std::endl;
}
