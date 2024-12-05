#include "../includes/Global.hpp"

std::string trimNewline(const std::string &str)
{
  size_t end = str.find_last_not_of("\r\n ");
  return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

bool parseMessage(const std::string &msg, std::string &command,
                  std::string &channel, std::string &parameters)
{
  std::istringstream stream(msg);
  if (!(stream >> command))
  {
    return false;
  }
  if (!(stream >> channel))
  {
    return false;
  }
  if (channel.find_first_of("\r\n") != std::string::npos)
  {
    channel = trimNewline(channel);
  }
  if (channel.find("\n") != std::string::npos)
  {
    channel = channel.substr(0, channel.length() - 1);
  }
  std::getline(stream, parameters);
  if (!parameters.empty() && parameters[0] == ' ')
  {
    parameters.erase(0, 1);
  }
  if (parameters.find_first_not_of(" \r\n") == std::string::npos)
  {
    parameters.clear();
  }
  if (parameters[0] == ':')
  {
    parameters.erase(0, 1);
  }
  return true;
}

std::string parseChannelName(const std::string &line)
{
  const std::string prefix = "PRIVMSG ";
  if (line.find(prefix) != 0)
  {
    std::cerr << "Invalid message format" << std::endl;
    return "";
  }
  std::string rest = line.substr(prefix.length());
  size_t space_pos = rest.find(' ');
  if (space_pos == std::string::npos)
  {
    std::cerr << "Invalid message format" << std::endl;
    return "";
  }
  std::string channel = rest.substr(0, space_pos);
  return channel;
}

void clearVector(std::vector<Client*>& vec) {
    for (std::vector<Client*>::iterator it = vec.begin(); it != vec.end(); ) {
        it = vec.erase(it);
    }
    vec.clear();
}

bool check_invalid_chars(const std::string &str)
{
    if (str.empty())
        return false;
    char first_char = str[0];
    if (first_char == '$' || first_char == ':' ||
        first_char == '#' || first_char == '&' ||
        first_char == '+' || first_char == '!')
        {
        return false;
    }
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (c == ' ' || c == ',' || c == '*' || c == '?' || c == '!' || c == '@' || c == '.') {
            return false;
        }
    }
    for (size_t i = 0; i < str.length(); ++i)
    {
        char c = str[i];
        if (!(std::isalnum(c) || c == '_'))
            return false;
    }
    return true;
}

void print_t_input(t_input *input)
{
  std::cerr << "Command: " << input->command << std::endl;
  std::cerr << "Channel: " << input->channel << std::endl;
  std::cerr << "Client : " << input->client->getNickname() << std::endl;
  std::cerr << "Parameters: " << input->parameters << std::endl;
  std::cerr << "Msg: " << input->msg << std::endl;
  std::cerr << "Prefix: " << input->prefix << std::endl;
}
