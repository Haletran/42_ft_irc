#include "../includes/Global.hpp"

void Server::JoinCommand(t_input *input)
{
  std::vector<std::string> _channels_input;
  std::stringstream ss(input->channel);
  std::string channel;
  while (std::getline(ss, channel, ','))
  {
    _channels_input.push_back(channel);
  }
  for (std::vector<std::string>::iterator it = _channels_input.begin(); it != _channels_input.end(); ++it)
    JoinChannel(*it, input->client, input->parameters);
}

void Server::InviteCommand(t_input *input)
{
  Client *client = get_ClientByNickname(input->channel);
  std::string channel_name = trimNewline(input->parameters);
  Channel *channel_instance = getChannelByName(channel_name);
  if (!channel_instance)
  {
	input->client->SendMsg("Channel does not exist.\r\n");
	return;
  }
  if (!client)
  {
    client = get_ClientByNickname(trimNewline(input->parameters));
    if (!client)
    {
      input->client->SendMsg(INVITE_USER_ERROR);
      return;
    }
  }
    if (channel_instance->getInvite() == true)
    {
        if (channel_instance->IsOP(input->client))
        {
            input->client->SendMsg(INVITE_SUCCESS_MSG);
            client->SendMsg(INVITE_MSG_NEW);
            if (channel_instance == NULL)
            {
                std::cerr << "Channel is NULL" << std::endl;
                return;
            }
            channel_instance->addInvited(client);
        }
        else
        {
            input->client->SendMsg(NOT_CHANNEL_OPERATOR_MSG);
        }
    }
    else if (channel_instance->getInvite() == false) {
        input->client->SendMsg(INVITE_SUCCESS_MSG);
        client->SendMsg(INVITE_MSG_NEW);
        if (channel_instance == NULL)
        {
            std::cerr << "Channel is NULL" << std::endl;
            return;
        }
        channel_instance->addInvited(client);
    }
    else
    {
      std::string msg = "User " + client->GetNick() + " is already connected to the channel.\r\n";
      input->client->SendMsg(msg);
    }
}

void Server::KickCommand(t_input *input)
{
  if (getChannelByName(input->channel) != NULL)
  {
    if (getChannelByName(input->channel)->IsOP(input->client) == false)
      input->client->SendMsg(NOT_OP);
    else
      KickFromChannel(input->channel, input->parameters, input->client);
  }
}

void Server::MsgCommand(t_input *input)
{
  std::string channel = parseChannelName(input->msg);
  std::string msg_content = input->msg.substr(input->msg.find(":", 1) + 1);
  if (getChannelByName(channel) != NULL)
    SendMessageToChannel(channel, input->client, msg_content);
  else
    SendMessageToSomeone(input->client, msg_content, channel);
}

void Server::TopicCommand(t_input *input)
{
  if (input ->channel[0] != '#')
    return;
  Channel *chan = getChannelByName(input->channel);
  if (!chan)
  {
	input->client->SendMsg("Channel does not exist.\r\n");
	return;
  }
  print_t_input(input);
  // topic without params to get the topic
  if (input->parameters.empty())
  {
      if (chan->getTopic().empty())
      {
          input->client->SendMsg(EMPTY_TOPIC);
          return;
      }
      input->client->SendMsg(TOPIC_MSG);
      return ;
  }
  // topic change if topic flag activated
  if (chan->getTopicChange() == true)
  {
      if (!chan->IsOP(input->client))
      {
          input->client->SendMsg(NOT_OP);
          return;
      }
      input->client->SendMsg(SET_TOPIC);
      SendMessageToChannel(input->channel, input->client, SET_TOPIC);
      for (std::vector<Client *>::iterator it = _channels[chan].begin();
           it != _channels[chan].end(); ++it)
      {
        if ((*it) != input->client)
          (*it)->SendMsg(SET_TOPIC);
      }
      chan->setTopic(input->parameters);
      return;
  }

  // topic change if topic flag desactived
  if (chan->getTopicChange() == false)
  {
      input->client->SendMsg(SET_TOPIC);
      for (std::vector<Client *>::iterator it = _channels[chan].begin();
           it != _channels[chan].end(); ++it)
      {
        if ((*it) != input->client)
          (*it)->SendMsg(SET_TOPIC);
      }
      chan->setTopic(input->parameters);
      return ;
  }
}

bool Server::ParsingMode(t_input *input, int & flag, Channel *channel)
{
    std::string mode_array = "itklo";

    if (input->parameters.empty())
        return false;
    if (!channel)
        return false;
    if (!channel->IsOP(input->client))
    {
        input->client->SendMsg(NOT_OP);
        return false;
    }
    std::string flag_str = trimNewline(input->parameters.substr(0, 3));
    std::string params_str = trimNewline(input->parameters.substr(2));
    flag_str.erase(flag_str.find_last_not_of(" \n\r\t") + 1);
    flag_str.erase(0, flag_str.find_first_not_of(" \n\r\t"));
    params_str.erase(params_str.find_last_not_of(" \n\r\t") + 1);
    params_str.erase(0, params_str.find_first_not_of(" \n\r\t"));

    if (flag_str.size() != 2)
        return false;
    for (int i = 0; i < static_cast<int>(mode_array.length()); ++i)
    {
        if (mode_array[i] == flag_str[1])
        {
            flag = i;
            break;
        }
    }
    if (flag == -1)
        return false;
    input->parameters = params_str;
    input->msg = flag_str;
    return true;
}


void Server::ModeCommand(t_input *input)
{
    int flag = -1;
    Channel* channel = getChannelByName(input->channel);
    if (ParsingMode(input, flag, channel) == false)
        return ;
    std::string modeMsg = MODE_MSG_NEW;
    if (input->msg[0] == '+')
    {
        switch (flag)
        {
            case 0: // +i
                channel->setInviteOnly(true);
                modeMsg = modeMsg + "+i";
                break;
            case 1: // +t
                channel->setTopicChange(false);
                modeMsg = modeMsg + "+t";
                break;
            case 2: // +k
                if (input->parameters.length() <= 0)
                {
                    input->client->SendMsg(KEY_MODE_ERROR);
                    return;
                }
                else
                {
                    channel->setPassword(trimNewline(input->parameters));
                    channel->setPasswordNeeded(true);
                    modeMsg = modeMsg + "+k " + input->parameters;
                }
                break;
            case 3: // +l
                if (input->parameters.length() <= 0)
                {
                    input->client->SendMsg(LIMIT_MODE_ERROR);
                    return;
                }
                else
                {
                    std::stringstream ss;
                    int limit = atoi(input->parameters.c_str());
                    if (limit <= 0)
                    {
                        input->client->SendMsg(INVALID_LIMIT_VALUE_MSG);
                        return;
                    }
                    channel->setUserLimit(limit);
                    ss << limit;
                    modeMsg = modeMsg + "+l " + ss.str();
                }
                break;
            case 4: // +o
                if (input->parameters.length() <= 0)
                {
                    input->client->SendMsg(OPERATOR_MODE_ERROR);
                    return;
                }
                else
                {
                    std::string username = trimNewline(input->parameters);
                    Client* target = get_ClientByNickname(username);
                    if (!target)
                    {
                        input->client->SendMsg(NO_SUCH_NICK_CHANNEL);
                        return;
                    }
                    channel->addOperators(target);
                    modeMsg = modeMsg + "+o " + username;
                }
                break;
        }
    }
    else if (input->msg[0] == '-')
    {
        switch (flag)
        {
            case 0:
                channel->setInviteOnly(false);
                modeMsg = modeMsg + "-i";
                break;
            case 1:
                channel->setTopicChange(true);
                modeMsg = modeMsg + "-t";
                break;
            case 2:
                if (input->parameters.length() <= 0)
                {
                    input->client->SendMsg(NOT_ENOUGH_PARAMETERS_MSG);
                    return;
                }
                if (trimNewline(input->parameters) == channel->getPassword())
                {
                    channel->setPasswordNeeded(false);
                    modeMsg = modeMsg + "-k";
                }
                else
                {
                    input->client->SendMsg(INVALID_PASSWORD_MSG);
                    return;
                }
                break;
            case 3:
                channel->setUserLimit(std::numeric_limits<int>::max());
                modeMsg = modeMsg + "-l";
                break;
            case 4:
                if (input->parameters.length() <= 0)
                {
                    input->client->SendMsg(NOT_ENOUGH_PARAMETERS_FOR_O_MSG);
                    return;
                }
                else
                {
                    std::string username = trimNewline(input->parameters);
                    Client* target = get_ClientByNickname(username);
                    if (!target)
                    {
                        input->client->SendMsg(NO_SUCH_NICK_CHANNEL);
                        return;
                    }
                    channel->removeOperator(target);
                    modeMsg = modeMsg + "-o " + username;
                }
                break;
        }
    }
    std::vector<Client*>::iterator it;
    for (it = _channels[channel].begin(); it != _channels[channel].end(); ++it)
    {
        (*it)->SendMsg(modeMsg + "\r\n");
    }
}

void Server::PartCommand(t_input *input)
{
  Channel *channel = getChannelByName(input->channel);
  if (channel == NULL)
  {
	input->client->SendMsg("Channel does not exist.\r\n");
	return;
  }
  if (!getCurrentChannel(input->client))
  	return;
  input->client->SendMsg(PART_MSG);
  if (_channels[channel].size() == 1)
  {
    _channels.erase(channel);
    delete channel;
    return;
  }
  for (std::vector<Client *>::iterator it = _channels[channel].begin();
       it != _channels[channel].end(); ++it)
  {
    if ((*it) != input->client)
      (*it)->SendMsg(PART_MSG);
  }
  channel->removeClient(input->client);
  channel->removeOperator(input->client);
  std::vector<Client *> &channelClients = _channels[channel];
  channelClients.erase(
      std::remove(channelClients.begin(), channelClients.end(), input->client),
      channelClients.end());
  input->client->SendMsg(PART_MSG);
}

void Server::JoinChannel(const std::string &channel_name, Client *client, std::string parameters)
{
  if (!client->GetAuth() || client->GetNick().empty() || client->GetUsername().empty())
  {
    std::cerr << "Client is not fully authenticated: " << client->GetFd() << std::endl;
    return;
  }
  std::string trimmed_channel_name = trimNewline(channel_name);
  Channel *channel = getChannelByName(trimmed_channel_name);
  if (channel != NULL)
  {
    if (channel->getInvite() == true && channel->IsInvited(client) == false)
    {
      client->SendMsg(INVITE_ONLY_ERROR);
      return;
    }
    else if (channel->getPasswordNeeded() == true)
    {
      if (trimNewline(parameters).compare(getChannelByName(channel_name)->getPassword()) != 0)
      {
        client->SendMsg(BAD_KEY_ERROR);
        return;
      }
    }
    else if (channel->getNbUser() + 1 >= channel->getlimit())
    {
      client->SendMsg(CHANNEL_FULL_ERROR);
      return;
    }
    else if (channel->isAlreadyConnected(client))
    {
      client->SendMsg(ALREADY_IN_CHANNEL_ERROR);
      return;
    }
    _channels[channel].push_back(client);
    for (std::vector<Client *>::iterator it = _channels[channel].begin(); it != _channels[channel].end(); ++it)
      (*it)->SendMsg(JOIN_MSG);
    channel->getAllUser(client);
    client->SendMsg(NEW_CHANNEL_MSG);
    client->SendMsg(END_OF_NAMES_MSG);
  }
  else
  {
    try
    {
      Channel *channel = new Channel(trimmed_channel_name);
      _channels[channel].push_back(client);
      channel->addOperators(client);
      client->SendMsg(JOIN_MSG);
      client->SendMsg(NEW_CHANNEL_MSG);
      client->SendMsg(END_OF_NAMES_MSG);
    }
    catch (Channel::ChannelException &e)
    {
      std::cerr << e.what() << std::endl;
      std::string errMsg = "Error: " + std::string(e.what()) + "\r\n";
      client->SendMsg(errMsg);
      return;
    }
  }
}

void Server::SendQuittingMessage(Client *client)
{
  Channel *channel = getCurrentChannel(client);
  if (channel != NULL)
  {
    while (getCurrentChannel(client))
    {
      Channel *channel = getCurrentChannel(client);
      std::vector<Client *> &clients = _channels[channel];
      for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
      {
        if (*it == client)
        {
          channel->removeClient(*it);
          channel->removeOperator(*it);
          clients.erase(it);
          break;
        }
      }
      for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
      {
        (*it)->SendMsg(QUIT_MSG);
      }
      channel->removeClient(client);
      channel->removeOperator(client);
      if (channel->getNbUser() == 0)
      {
        _channels.erase(channel);
        delete channel;
      }
    }
  }
}

void Server::KickFromChannel(const std::string &channel,
                             const std::string &nickname, Client *client)
{
  std::string nick = trimNewline(nickname);
  Channel *channelPtr = getChannelByName(channel);
  if (channelPtr == NULL)
  {
    client->SendMsg(CHANNEL_NOT_FOUND);
    return;
  }
  std::vector<Client *> &clientsInChannel = _channels[channelPtr];
  for (std::vector<Client *>::iterator clientIt = clientsInChannel.begin();
       clientIt != clientsInChannel.end(); ++clientIt)
  {
    if ((*clientIt)->GetNick() == nick)
    {
      for (std::vector<Client *>::iterator notifyIt = clientsInChannel.begin();
           notifyIt != clientsInChannel.end(); ++notifyIt)
      {
        (*notifyIt)->SendMsg(KICK_MSG);
      }
      LeaveChannel(channel, *clientIt);
      return;
    }
  }
  client->SendMsg(USER_NOT_ON_CHANNEL);
}
