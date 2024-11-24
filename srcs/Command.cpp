#include "../includes/Global.hpp"

void Server::JoinCommand(t_input *input)
{
  if (input->channel[input->channel.length() - 1] == '\n')
    input->channel = input->channel.substr(0, input->channel.length() - 1);
  JoinChannel(input->channel, input->client, input->parameters);
}

void Server::InviteCommand(t_input *input)
{
  Client *test = get_ClientByNickname(input->channel);
  Channel *channel_instance = getCurrentChannel(input->client);
  if (!test)
  {
    test = get_ClientByNickname(trimNewline(input->parameters));
    if (!test)
    {
      input->client->SendMsg(INVITE_USER_ERROR);
      return;
    }
  }
  if (channel_instance->isAlreadyConnected(test) == false)
  {
    input->client->SendMsg(INVITE_SUCCESS_MSG);
    test->SendMsg(INVITE_MSG_NEW);
    if (channel_instance == NULL)
    {
      std::cerr << "Channel is NULL" << std::endl;
      return;
    }
    channel_instance->addInvited(test);
  }
  else
  {
    std::string msg = "User " + test->GetNick() + " is already connected to the channel.\r\n";
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
  {
    return;
  }
  Channel *chan = getChannelByName(input->channel);
  if (chan->getTopicChange() == false && chan->IsOP(input->client))
  {
    if (input->parameters.empty())
    {
      std::string currentTopic = getChannelByName(input->channel)->getTopic();
      if (currentTopic.empty())
      {
        input->client->SendMsg(EMPTY_TOPIC);
      }
      else
      {
        input->client->SendMsg(TOPIC_ERROR);
      }
    }
    else
    {
      getChannelByName(input->channel)->setTopic(input->parameters);
      for (std::vector<Client *>::iterator it =
               _channels[getChannelByName(input->channel)].begin();
           it != _channels[getChannelByName(input->channel)].end(); ++it)
        (*it)->SendMsg(SET_TOPIC);
    }
  }
  else if (chan->getTopicChange() == true)
  {
    if (input->parameters.empty())
    {
      std::string currentTopic = getChannelByName(input->channel)->getTopic();
      if (currentTopic.empty())
      {
        input->client->SendMsg(EMPTY_TOPIC);
      }
      else
      {
        input->client->SendMsg(TOPIC_ERROR);
      }
    }
    else
    {
      getChannelByName(input->channel)->setTopic(input->parameters);
      for (std::vector<Client *>::iterator it =
               _channels[getChannelByName(input->channel)].begin();
           it != _channels[getChannelByName(input->channel)].end(); ++it)
        (*it)->SendMsg(SET_TOPIC);
    }
  }
  else
  {
    input->client->SendMsg(NOT_OP);
  }
}

/* void Server::ModeCommand(t_input *input)
{
  std::string mode_array = "itklo";
  int flag = -1;
  if (input->parameters.empty())
    return;
  if (getChannelByName(input->channel) == NULL)
    return;
  if (getChannelByName(input->channel)->IsOP(input->client) == false)
  {
    input->client->SendMsg(NOT_OP);
    return;
  }
  if (!input->parameters.empty())
  {
    for (size_t i = 0; i < mode_array.size(); i++)
    {
      if (input->parameters[1] == mode_array[i])
      {
        flag = i;
        break;
      }
    }
  }
  if (flag == -1)
    return;
  if (input->parameters.size() > 1 && input->parameters.at(0) == '+')
  {
    switch (flag)
    {
    case 0:
      getChannelByName(input->channel)->setInviteOnly(true);
      break;
    case 1:
      getChannelByName(input->channel)->setTopicChange(false);
      break;
    case 2:
      getChannelByName(input->channel)->setPassword(input->parameters.substr(3));
      getChannelByName(input->channel)->setPasswordNeeded(true);
      break;
    case 3:
      getChannelByName(input->channel)->setUserLimit(atoi(input->parameters.substr(3).c_str()));
      break;
    case 4:
      std::string clientUsername = trimNewline(input->parameters.substr(3));
      if (get_ClientByNickname(clientUsername) != NULL)
        getChannelByName(input->channel)->addOperators(get_ClientByNickname(clientUsername));
      else
        return;
      break;
    }
  }
  else if (input->parameters.size() > 1 && input->parameters.at(0) == '-')
  {
    switch (flag)
    {
    case 0:
      getChannelByName(input->channel)->setInviteOnly(false);
      break;
    case 1:
      getChannelByName(input->channel)->setTopicChange(true);
      break;
    case 2:
      if (input->parameters.substr(3) == getChannelByName(input->channel)->getPassword())
        getChannelByName(input->channel)->setPasswordNeeded(false);
      else
        return;
      break;
    case 3:
      getChannelByName(input->channel)->setUserLimit(std::numeric_limits<int>::max());
      break;
    case 4:
      std::string clientUsername = trimNewline(input->parameters.substr(3));
      if (get_ClientByNickname(clientUsername) != NULL)
        getChannelByName(input->channel)->removeOperator(get_ClientByNickname(clientUsername));
      else
        return;
      break;
    }
  }
  for (std::vector<Client *>::iterator it =
           _channels[getChannelByName(input->channel)].begin();
       it != _channels[getChannelByName(input->channel)].end(); ++it)
    (*it)->SendMsg(MODE_MESSAGE);
} */

/* void Server::ModeCommand(t_input *input)
{
    std::string mode_array = "itklo";
    int flag = -1;
    
    if (input->parameters.empty())
        return;  
    Channel* channel = getChannelByName(input->channel);
    if (!channel)
        return;  
    if (!channel->IsOP(input->client))
    {
        input->client->SendMsg(NOT_OP);
        return;
    }
    if (input->parameters.length() > 1)
    {
        char mode_char = input->parameters[1];
        for (int i = 0; i < static_cast<int>(mode_array.length()); ++i)
        {
            if (mode_array[i] == mode_char)
            {
                flag = i;
                break;
            }
        }
    }

    if (flag == -1)
        return;
    std::string modeMsg = ":";
    modeMsg = modeMsg + input->client->GetNick() + " MODE " + channel->GetChannelName() + " ";
    if (input->parameters.length() > 1 && input->parameters[0] == '+')
    {
        switch (flag)
        {
            case 0:
                channel->setInviteOnly(true);
                modeMsg = modeMsg + "+i";
                break;
            case 1:
                channel->setTopicChange(false);
                modeMsg = modeMsg + "+t";
                break;
            case 2:
                if (input->parameters.length() > 3)
                {
                    std::string pass = input->parameters.substr(3);
                    channel->setPassword(pass);
                    channel->setPasswordNeeded(true);
                    modeMsg = modeMsg + "+k " + pass;
                }
                break;
            case 3:
                if (input->parameters.length() > 3)
                {
                    std::stringstream ss;
                    int limit = atoi(input->parameters.substr(3).c_str());
                    channel->setUserLimit(limit);
                    ss << limit;
                    modeMsg = modeMsg + "+l " + ss.str();
                }
                break;
            case 4:
                if (input->parameters.length() > 3)
                {
                    std::string username = trimNewline(input->parameters.substr(3));
                    Client* target = get_ClientByNickname(username);
                    if (target)
                    {
                        channel->addOperators(target);
                        modeMsg = modeMsg + "+o " + username;
                    }
                }
                break;
        }
    }
    else if (input->parameters.length() > 1 && input->parameters[0] == '-')
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
                if (input->parameters.length() > 3 && 
                    input->parameters.substr(3) == channel->getPassword())
                {
                    channel->setPasswordNeeded(false);
                    modeMsg = modeMsg + "-k";
                }
                break;
            case 3:
                channel->setUserLimit(std::numeric_limits<int>::max());
                modeMsg = modeMsg + "-l";
                break;
            case 4:
                if (input->parameters.length() > 3)
                {
                    std::string username = trimNewline(input->parameters.substr(3));
                    Client* target = get_ClientByNickname(username);
                    if (target)
                    {
                        channel->removeOperator(target);
                        modeMsg = modeMsg + "-o " + username;
                    }
                }
                break;
        }
    }
    std::vector<Client*>::iterator it;
    for (it = _channels[channel].begin(); it != _channels[channel].end(); ++it)
    {
        (*it)->SendMsg(modeMsg + "\r\n");
    }
} */

void Server::ModeCommand(t_input *input)
{
    std::string mode_array = "itklo";
    int flag = -1;
    
    if (input->parameters.empty())
        return;  
    Channel* channel = getChannelByName(input->channel);
    if (!channel)
        return;  
    if (!channel->IsOP(input->client))
    {
        input->client->SendMsg(NOT_OP);
        return;
    }
    if (input->parameters.length() > 1)
    {
        char mode_char = input->parameters[1];
        for (int i = 0; i < static_cast<int>(mode_array.length()); ++i)
        {
            if (mode_array[i] == mode_char)
            {
                flag = i;
                break;
            }
        }
    }

    if (flag == -1)
        return;
    std::string modeMsg = ":" + input->client->GetNick() + "!" + input->client->GetUsername() + "@localhost MODE " + channel->GetChannelName() + " ";
    if (input->parameters.length() > 1 && input->parameters[0] == '+')
    {
        switch (flag)
        {
            case 0:
                channel->setInviteOnly(true);
                modeMsg = modeMsg + "+i";
                break;
            case 1:
                channel->setTopicChange(false);
                modeMsg = modeMsg + "+t";
                break;
            case 2:
                if (input->parameters.length() <= 3)
                {
                    input->client->SendMsg(":" + channel->GetChannelName() + " 696 " + input->client->GetNick() + " " + channel->GetChannelName() + " k * :You must specify a parameter for the key mode\r\n");
                    return;
                }
                else
                {
                    std::string pass = input->parameters.substr(3);
                    channel->setPassword(pass);
                    channel->setPasswordNeeded(true);
                    modeMsg = modeMsg + "+k " + pass;
                }
                break;
            case 3:
                if (input->parameters.length() <= 3)
                {
                    input->client->SendMsg(":" + channel->GetChannelName() + " 696 " + input->client->GetNick() + " " + channel->GetChannelName() + " l * :You must specify a parameter for the limit mode\r\n");
                    return;
                }
                else
                {
                    std::stringstream ss;
                    int limit = atoi(input->parameters.substr(3).c_str());
                    if (limit <= 0)
                    {
                        input->client->SendMsg(":server 461 " + input->client->GetNick() + " MODE :Invalid limit value\r\n");
                        return;
                    }
                    channel->setUserLimit(limit);
                    ss << limit;
                    modeMsg = modeMsg + "+l " + ss.str();
                }
                break;
            case 4:
                if (input->parameters.length() <= 3)
                {
                    input->client->SendMsg(":" + channel->GetChannelName() + " 696 " + input->client->GetNick() + " " + channel->GetChannelName() + " o * :You must specify a parameter for the operator mode\r\n");
                    return;
                }
                else
                {
                    std::string username = trimNewline(input->parameters.substr(3));
                    Client* target = get_ClientByNickname(username);
                    if (!target)
                    {
                        input->client->SendMsg(":server 401 " + input->client->GetNick() + " " + username + " :No such nick\r\n");
                        return;
                    }
                    channel->addOperators(target);
                    modeMsg = modeMsg + "+o " + username;
                }
                break;
        }
    }
    else if (input->parameters.length() > 1 && input->parameters[0] == '-')
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
                if (input->parameters.length() <= 3)
                {
                    input->client->SendMsg(":server 461 " + input->client->GetNick() + " MODE :Not enough parameters for -k\r\n");
                    return;
                }
                if (input->parameters.substr(3) == channel->getPassword())
                {
                    channel->setPasswordNeeded(false);
                    modeMsg = modeMsg + "-k";
                }
                else
                {
                    input->client->SendMsg(":server 464 " + input->client->GetNick() + " :Invalid password\r\n");
                    return;
                }
                break;
            case 3:
                channel->setUserLimit(std::numeric_limits<int>::max());
                modeMsg = modeMsg + "-l";
                break;
            case 4:
                if (input->parameters.length() <= 3)
                {
                    input->client->SendMsg(":server 461 " + input->client->GetNick() + " MODE :Not enough parameters for -o\r\n");
                    return;
                }
                else
                {
                    std::string username = trimNewline(input->parameters.substr(3));
                    Client* target = get_ClientByNickname(username);
                    if (!target)
                    {
                        input->client->SendMsg(":server 401 " + input->client->GetNick() + " " + username + " :No such nick\r\n");
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
      if (parameters.compare(getChannelByName(channel_name)->getPassword()) != 0)
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
  std::string msg = ":" + client->GetNick() + "!~" + client->GetUsername() + "@localhost QUIT :Client Quit\r\n";
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
        (*it)->SendMsg(msg);
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
