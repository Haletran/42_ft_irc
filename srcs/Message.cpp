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
  struct s_input *input = new struct s_input;
  input->command = command;
  input->channel = channel;
  input->parameters = parameters;
  input->msg = msg;
  input->client = client;
  executeCommand(input);
}

void Server::executeCommand(t_input *input)
{
  switch (GetCommand(input->command))
  {
  case 0: // JOIN
    JoinCommand(input);
    break;
  case 1: // INVITE
  {
    InviteCommand(input);
    break;
  }
  case 2: // KICK
    KickCommand(input);
    break;
  case 3: // PRIVMSG
  {
    MsgCommand(input);
    break;
  }
  case 4: // TOPIC
  {
    TopicCommand(input);
    break;
  }
  case 5: // MODE
  {
    ModeCommand(input);
    break;
  }
  case 6: // PART
  {
    PartCommand(input);
    break;
  }
  case 7:
  {
    SendInfos(input->channel, input->client);
    break;
  }
  case 8: // QUIT
  {
    close(input->client->GetFd());
    ClearClients(input->client->GetFd());
    break;
  }
  default:
    std::cerr << "Invalid command" << std::endl;
    break;
  }
  delete input;
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
  for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
  {
    if ((*it)->getNickname() == nickname)
    {
      (*it)->SendMsg(PRIV_MSG);
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
