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
  input->command = trimNewline(command);
  input->channel = trimNewline(channel);
  input->parameters = trimNewline(parameters);
  input->msg = trimNewline(msg);
  input->client = client;
  executeCommand(input);
}

void Server::executeCommand(t_input *input)
{
  if (_commands.find(input->command) != _commands.end()) {
      (this->*(_commands[input->command]))(input);
  } else {
        std::cerr << "Command not found: " << input->command << std::endl;
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
