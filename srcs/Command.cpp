#include "../includes/Global.hpp"

void Server::JoinCommand(t_input *input)
{
    if (input->channel[input->channel.length() - 1] == '\n')
      input->channel = input->channel.substr(0, input->channel.length() - 1);
    JoinChannel(input->channel, input->client, input->parameters);
}

void Server::InviteCommand(t_input *input)
{
    Client *test = get_ClientByUsername(input->channel);
    Channel *channel_instance = getCurrentChannel(input->client);
    if (!test)
      input->client->SendMsg(INVITE_USER_ERROR);
    else if (channel_instance->isAlreadyConnected(test) == false)
    {
      input->client->SendMsg(INVITE_SUCCESS_MSG);
      test->SendMsg(INVITE_MSG_NEW);
      if (channel_instance == NULL)
      {
        std::cerr << "Channel is NULL" << std::endl;
        return ;
      }
      channel_instance->addInvited(test);
    }
}

void Server::KickCommand(t_input *input)
{
    if (getChannelByName(input->channel)->IsOP(input->client) == false)
      input->client->SendMsg(NOT_OP);
    else
      KickFromChannel(input->channel, input->parameters, input->client);
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


void Server::ModeCommand(t_input *input)
{
    std::string mode_array = "itklo";
    int flag = -1;
    if (input->parameters.empty())
      return ;
    if (getChannelByName(input->channel) == NULL)
      return ;
    if (getChannelByName(input->channel)->IsOP(input->client) == false)
    {
      input->client->SendMsg(NOT_OP);
      return ;
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
        getChannelByName(input->channel)->setUserLimit(
            atoi(input->parameters.substr(3).c_str()));
        break;
      case 4:
        std::string clientUsername = trimNewline(input->parameters.substr(3));
        if (get_ClientByNickname(clientUsername) != NULL)
            getChannelByName(input->channel)->addOperators(get_ClientByNickname(clientUsername));
        else
            return ;
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
            return ;
        break;
      }
      // this is not working it's for MODE command that hexchat send when JOIN a channel
    }
    for (std::vector<Client *>::iterator it =
             _channels[getChannelByName(input->channel)].begin();
         it != _channels[getChannelByName(input->channel)].end(); ++it)
      (*it)->SendMsg(MODE_MESSAGE);
}


void Server::PartCommand(t_input *input)
{
    if (_channels[getChannelByName(input->channel)].size() == 1)
        return ;
    for (std::vector<Client *>::iterator it =
             _channels[getChannelByName(input->channel)].begin();
         it != _channels[getChannelByName(input->channel)].end(); ++it)
    {
      if ((*it) != input->client)
        (*it)->SendMsg(PART_MSG);
    }
}
