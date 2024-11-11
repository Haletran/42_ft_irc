#include "../includes/Global.hpp"

void Server::ProcedeCommand(const std::string &msg, Client *client)
{
  std::string command, channel, parameters;
  if (!parseMessage(msg, command, channel, parameters))
  {
    std::cerr << "Invalid message format" << std::endl;
    return;
  }
  std::cerr << RECEIVE_DEBUG << std::endl;
  executeCommand(command, channel, client, parameters, msg);
}

void Server::executeCommand(std::string command, std::string channel,
                            Client *client, std::string parameters,
                            std::string msg)
{
  switch (GetCommand(command))
  {
  case 0: // JOIN
    if (channel[channel.length() - 1] == '\n')
      channel = channel.substr(0, channel.length() - 1);
    JoinChannel(channel, client, parameters);
    break;
  case 1: // INVITE
  {
    Client *test = get_ClientByUsername(channel);
    Channel *channel_instance = getCurrentChannel(client);
    if (!test)
    {
      client->SendMsg(INVITE_USER_ERROR);
    }
    else if (channel_instance->isAlreadyConnected(test) == false)
    {
      client->SendMsg(INVITE_SUCCESS_MSG);
      test->SendMsg(INVITE_MSG_NEW);
      if (channel_instance == NULL)
      {
        std::cerr << "Channel is NULL" << std::endl;
        break;
      }
      channel_instance->addInvited(test);
    }
    break;
  }
  case 2: // KICK
    if (getChannelByName(channel)->IsOP(client) == false)
      client->SendMsg(NOT_OP);
    else
      KickFromChannel(channel, parameters, client);
    break;
  case 3: // PRIVMSG
  {
    std::string channel = parseChannelName(msg);
    std::string msg_content = msg.substr(msg.find(":", 1) + 1);
    if (getChannelByName(channel) != NULL)
      SendMessageToChannel(channel, client, msg_content);
    else
      SendMessageToSomeone(client, msg_content, channel);
    break;
  }
  case 4: // TOPIC
  {
    Channel *chan = getChannelByName(channel);
    if (chan->getTopicChange() == false && chan->IsOP(client))
    {
      if (parameters.empty())
      {
        std::string currentTopic = getChannelByName(channel)->getTopic();
        if (currentTopic.empty())
        {
          client->SendMsg(EMPTY_TOPIC);
        }
        else
        {
          client->SendMsg(TOPIC_ERROR);
        }
      }
      else
      {
        getChannelByName(channel)->setTopic(parameters);
        for (std::vector<Client *>::iterator it =
                 _channels[getChannelByName(channel)].begin();
             it != _channels[getChannelByName(channel)].end(); ++it)
          (*it)->SendMsg(SET_TOPIC);
      }
    }
    else if (chan->getTopicChange() == true)
    {
      if (parameters.empty())
      {
        std::string currentTopic = getChannelByName(channel)->getTopic();
        if (currentTopic.empty())
        {
          client->SendMsg(EMPTY_TOPIC);
        }
        else
        {
          client->SendMsg(TOPIC_ERROR);
        }
      }
      else
      {
        getChannelByName(channel)->setTopic(parameters);
        for (std::vector<Client *>::iterator it =
                 _channels[getChannelByName(channel)].begin();
             it != _channels[getChannelByName(channel)].end(); ++it)
          (*it)->SendMsg(SET_TOPIC);
      }
    }
    else
    {
      client->SendMsg(NOT_OP);
    }
    break;
  }
  case 5: // MODE
  {
    std::string mode_array = "itklo";
    int flag = -1;
    if (parameters.empty())
      break;
    if (getChannelByName(channel) == NULL)
      break;
    if (getChannelByName(channel)->IsOP(client) == false)
    {
      client->SendMsg(NOT_OP);
      break;
    }
    if (!parameters.empty())
    {
      for (size_t i = 0; i < mode_array.size(); i++)
      {
        if (parameters[1] == mode_array[i])
        {
          flag = i;
          break;
        }
      }
    }
    if (parameters.size() > 1 && parameters.at(0) == '+')
    {
      switch (flag)
      {
      case 0:
        getChannelByName(channel)->setInviteOnly(true);
        break;
      case 1:
        getChannelByName(channel)->setTopicChange(false);
        break;
      case 2:
        getChannelByName(channel)->setPassword(parameters.substr(3));
        getChannelByName(channel)->setPasswordNeeded(true);
        break;
      case 3:
        getChannelByName(channel)->setUserLimit(
            atoi(parameters.substr(3).c_str()));
        break;
      case 4:
        std::string clientUsername = trimNewline(parameters.substr(3));
        if (get_ClientByNickname(clientUsername) != NULL)
            getChannelByName(channel)->addOperators(get_ClientByNickname(clientUsername));
        else
            return ;
        break;
      }
    }
    else if (parameters.size() > 1 && parameters.at(0) == '-')
    {
      switch (flag)
      {
      case 0:
        getChannelByName(channel)->setInviteOnly(false);
        break;
      case 1:
        getChannelByName(channel)->setTopicChange(true);
        break;
      case 2:
        if (parameters.substr(3) == getChannelByName(channel)->getPassword())
          getChannelByName(channel)->setPasswordNeeded(false);
        else
          return;
        break;
      case 3:
        getChannelByName(channel)->setUserLimit(std::numeric_limits<int>::max());
        break;
      case 4:
        std::string clientUsername = trimNewline(parameters.substr(3));
        if (get_ClientByNickname(clientUsername) != NULL)
            getChannelByName(channel)->removeOperator(get_ClientByNickname(clientUsername));
        else
            return ;
        break;
      }
      // this is not working it's for MODE command that hexchat send when JOIN a channel
    }
    for (std::vector<Client *>::iterator it =
             _channels[getChannelByName(channel)].begin();
         it != _channels[getChannelByName(channel)].end(); ++it)
      (*it)->SendMsg(MODE_MESSAGE);
    break;
  }
  case 6: // PART
  {
    std::string part = ":" + client->GetUsername() + "!~" +
                       client->getNickname() + "@localhost PART " + channel +
                       "\r\n";
    if (_channels[getChannelByName(channel)].size() == 1)
    {
      //getChannelByName(channel)->CleanChannel(getChannelByName(channel));
      //_channels.erase(getChannelByName(channel));
      break;
    }
    // might need to delete the user of the channel if he leaves idk
    for (std::vector<Client *>::iterator it =
             _channels[getChannelByName(channel)].begin();
         it != _channels[getChannelByName(channel)].end(); ++it)
    {
      if ((*it) != client)
        (*it)->SendMsg(part);
    }
    break;
  }
  case 7:
  {
    SendInfos(channel, client);
    break;
  }
  case 8: // QUIT
  {
    close(client->GetFd());
    ClearClients(client->GetFd());
    break;
  }
  }
}

void Server::ProcedeMessage(const std::string &msg, Client *client)
{
  std::string channel;
  std::string message;
  std::istringstream stream(msg);
  std::string line;
  while (std::getline(stream, line))
  {
    ProcedeCommand(line, client);
  }
}

void Server::SendMessageToChannel(const std::string &channel_name,
                                  Client *sender, const std::string &message)
{
  Channel *channel = getChannelByName(channel_name);
  if (channel == NULL)
  {
    std::string errorMsg = "Channel " + channel_name + " does not exist.\r\n";
    sender->SendMsg(errorMsg);
    return;
  }
  for (std::vector<Client *>::iterator it = _channels[channel].begin();
       it != _channels[channel].end(); ++it)
  {
    if (*it != sender)
    {
      (*it)->SendMsg(FORMATTED_MESSAGE);
    }
  }
}

void Server::SendMessageToSomeone(Client *client, std::string msg_content, std::string nickname)
{
  std::string msg = ":" + client->GetUsername() + "!~" + client->getNickname() + "@localhost PRIVMSG " + nickname + " :" + msg_content + "\r\n";
  for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
  {
    if ((*it)->getNickname() == nickname)
    {
      (*it)->SendMsg(msg);
      return;
    }
  }
  client->SendMsg("User " + nickname + " not found.\r\n");
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

int Server::GetCommand(std::string command)
{
  if (command == "JOIN")
    return (0);
  if (command == "INVITE")
    return (1);
  if (command == "KICK")
    return (2);
  if (command == "PRIVMSG")
    return (3);
  if (command == "TOPIC")
    return (4);
  if (command == "MODE")
    return (5);
  if (command == "PART")
    return (6);
  if (command == "WHO")
    return (7);
  if (command == "QUIT")
    return (8);
  return (-1);
}
