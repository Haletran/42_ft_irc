#include "../includes/Global.hpp"
#include <cstdlib>
#include <exception>
#include <sstream>

std::string parseChannelName(const std::string &line) {
  const std::string prefix = "PRIVMSG ";
  if (line.find(prefix) != 0) {
    std::cerr << "Invalid message format" << std::endl;
    return "";
  }
  std::string rest = line.substr(prefix.length());
  size_t space_pos = rest.find(' ');
  if (space_pos == std::string::npos) {
    std::cerr << "Invalid message format" << std::endl;
    return "";
  }
  std::string channel = rest.substr(0, space_pos);
  return channel;
}

bool parseMessage(const std::string &msg, std::string &command,
                  std::string &channel, std::string &parameters) {
  std::istringstream stream(msg);
  if (!(stream >> command)) {
    return false;
  }
  if (!(stream >> channel)) {
    return false;
  }
  if (channel.find_first_of("\r\n") != std::string::npos) {
    channel = trimNewline(channel);
  }
  if (channel.find("\n") != std::string::npos) {
    channel = channel.substr(0, channel.length() - 1);
  }
  std::getline(stream, parameters);
  if (!parameters.empty() && parameters[0] == ' ') {
    parameters.erase(0, 1);
  }
  if (parameters.find_first_not_of(" \r\n") == std::string::npos) {
    parameters.clear();
  }
  if (parameters[0] == ':') {
    parameters.erase(0, 1);
  }
  return true;
}

void Server::ProcedeCommand(const std::string &msg, Client *client) {
  std::string command, channel, parameters;
  if (!parseMessage(msg, command, channel, parameters)) {
    std::cerr << "Invalid message format" << std::endl;
    return;
  }
  std::cerr << RECEIVE_DEBUG << std::endl;
  executeCommand(command, channel, client, parameters, msg);
}

void Server::executeCommand(std::string command, std::string channel,
                            Client *client, std::string parameters,
                            std::string msg) {
  switch (GetCommand(command)) {
  case 0: // JOIN
    if (channel[channel.length() - 1] == '\n')
      channel = channel.substr(0, channel.length() - 1);
    JoinChannel(channel, client, parameters);
    break;
  case 1: // INVITE
  {
    Client *test = get_ClientByUsername(channel);
    Channel *channel_instance = getCurrentChannel(client);
    if (!test) {
      client->SendMsg(INVITE_USER_ERROR);
    } else if (channel_instance->isAlreadyConnected(test) == false) {
      client->SendMsg(INVITE_SUCCESS_MSG);
      test->SendMsg(INVITE_MSG_NEW);
      if (channel_instance == NULL) {
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
    SendMessageToChannel(channel, client, msg_content);
    break;
  }
  case 4: // TOPIC
  {
    Channel *chan = getChannelByName(channel);
    if (chan->getTopicChange() == false && chan->IsOP(client)) {
      if (parameters.empty()) {
        std::string currentTopic = getChannelByName(channel)->getTopic();
        if (currentTopic.empty()) {
          client->SendMsg(EMPTY_TOPIC);
        } else {
          client->SendMsg(TOPIC_ERROR);
        }
      } else {
        getChannelByName(channel)->setTopic(parameters);
        for (std::vector<Client *>::iterator it =
                 _channels[getChannelByName(channel)].begin();
             it != _channels[getChannelByName(channel)].end(); ++it)
          (*it)->SendMsg(SET_TOPIC);
      }
    } else if (chan->getTopicChange() == true) {
      if (parameters.empty()) {
        std::string currentTopic = getChannelByName(channel)->getTopic();
        if (currentTopic.empty()) {
          client->SendMsg(EMPTY_TOPIC);
        } else {
          client->SendMsg(TOPIC_ERROR);
        }
      } else {
        getChannelByName(channel)->setTopic(parameters);
        for (std::vector<Client *>::iterator it =
                 _channels[getChannelByName(channel)].begin();
             it != _channels[getChannelByName(channel)].end(); ++it)
          (*it)->SendMsg(SET_TOPIC);
      }
    } else {
      client->SendMsg(NOT_OP);
    }
    break;
  }
  case 5: // MODE
  {
    std::string mode_array = "itkl";
    int flag = -1;
    if (parameters.empty())
      break;
    if (getChannelByName(channel) == NULL)
      break;
    if (getChannelByName(channel)->IsOP(client) == false) {
      client->SendMsg(NOT_OP);
      break;
    }
    if (!parameters.empty()) {
      for (size_t i = 0; i < mode_array.size(); i++) {
        if (parameters[1] == mode_array[i]) {
          flag = i;
          break;
        }
      }
    }
    if (parameters.size() > 1 && parameters.at(0) == '+') {
      switch (flag) {
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
      }
    } else if (parameters.size() > 1 && parameters.at(0) == '-') {
      switch (flag) {
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
        getChannelByName(channel)->setUserLimit(
            std::numeric_limits<int>::max());
        break;
      }
    } else if (channel.at(0) == '#') {
      std::string flag = getChannelByName(channel)->getFlag();
      if (!flag.empty()) {
        for (std::vector<Client *>::iterator it =
                 _channels[getChannelByName(channel)].begin();
             it != _channels[getChannelByName(channel)].end(); ++it)
          (*it)->SendMsg(FLAG_MSG);
      }
      return;
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
    // check if there is any user left in the channel if not then delete the channel
    // might need to delete the user of the channel if he leaves idk
    for (std::vector<Client *>::iterator it =
             _channels[getChannelByName(channel)].begin();
         it != _channels[getChannelByName(channel)].end(); ++it) {
      if ((*it) != client)
        (*it)->SendMsg(part);
    }
    break;
  }
  case 7: // QUIT
    ClearClients(client->GetFd());
  }
}

void Server::ProcedeMessage(const std::string &msg, Client *client) {
  std::string channel;
  std::string message;
  std::istringstream stream(msg);
  std::string line;
  while (std::getline(stream, line)) {
    ProcedeCommand(line, client);
  }
}

std::string trimNewline(const std::string &str) {
  size_t end = str.find_last_not_of("\r\n ");
  return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

void Server::SendMessageToChannel(const std::string &channel_name,
                                  Client *sender, const std::string &message) {
  Channel *channel = getChannelByName(channel_name);
  if (channel == NULL) {
    std::string errorMsg = "Channel " + channel_name + " does not exist.\r\n";
    sender->SendMsg(errorMsg);
    return;
  }
  for (std::vector<Client *>::iterator it = _channels[channel].begin();
       it != _channels[channel].end(); ++it) {
    if (*it != sender) {
      (*it)->SendMsg(FORMATTED_MESSAGE);
    }
  }
}

void Server::KickFromChannel(const std::string &channel,
                             const std::string &nickname, Client *client) {
  std::string nick = trimNewline(nickname);
  Channel *channelPtr = getChannelByName(channel);
  if (channelPtr == NULL) {
    client->SendMsg(CHANNEL_NOT_FOUND);
    return;
  }
  std::vector<Client *> &clientsInChannel = _channels[channelPtr];
  for (std::vector<Client *>::iterator clientIt = clientsInChannel.begin();
       clientIt != clientsInChannel.end(); ++clientIt) {
    if ((*clientIt)->GetNick() == nick) {
      for (std::vector<Client *>::iterator notifyIt = clientsInChannel.begin();
           notifyIt != clientsInChannel.end(); ++notifyIt) {
        (*notifyIt)->SendMsg(KICK_MSG);
      }
      LeaveChannel(channel, *clientIt);
      return;
    }
  }
  client->SendMsg(USER_NOT_ON_CHANNEL);
}

int Server::GetCommand(std::string command) {
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
  if (command == "QUIT")
    return (7);
  return (-1);
}
