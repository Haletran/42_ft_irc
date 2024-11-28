#pragma once

#define MODE_MESSAGE ":" + input->client->GetNick() + "!~" + input->client->GetUsername() + "@localhost MODE " + input->channel + " " + input->parameters + "\r\n"
#define FLAG_MSG ":localhost 324 " + client->getNickname() + " " + channel->GetChannelName() + " +" + channel->getFlag() + "\r\n"
#define MODE_JOIN ":localhost MODE " + channel->GetChannelName() + " +" + channel->getFlag() + "\r\n"
#define CREATION_TIME ":localhost 329 " + client->getNickname() + " " + channel_name  + " " + getChannelByName(channel_name)->getTimeCreated() + "\r\n"
#define SET_TOPIC ":" + input->client->GetNick() + "!" + input->client->GetUsername() + "@localhost TOPIC " + input->channel + " :" + input->parameters + "\r\n"
#define EMPTY_TOPIC ":localhost 331 " + input->client->GetNick() + " " + input->channel + " :No topic is set\r\n"
#define INVITE_MSG ":" + client->GetUsername() + " INVITE " + channel + " " + parameters
#define INVITE_USER_ERROR ": 401 " + input->client->GetNick() + " " + input->channel + " :No such nick\n"
#define INVITE_SUCCESS_MSG ":localhost 341 " + input->client->GetUsername() + " " + test->GetUsername() + " " + input->parameters + "\r\n"
#define INVITE_CONFIRM_MSG ": 341 " + input->client->GetUsername() + " " + input->test->GetUsername() + " #" + parameters + "\n"
#define INVITE_MSG_NEW ":" + input->client->getNickname() + "!~" + input->client->GetUsername() + "@localhost INVITE " + test->getNickname() + " :" + input->parameters + "\r\n"
#define FORMATTED_MESSAGE ":" + sender->GetNick() + "!" + sender->GetUsername() + "@localhost PRIVMSG " + channel_name + " :" + message + "\r\n"
#define CHANNEL_NOT_FOUND "403 " + client->GetNick() + " " + channel + " :No such channel\r\n"
#define KICK_MSG ":" + client->GetNick() + " KICK " + channel + " " + nick + " :Kicked by " + client->GetNick() + "\r\n"
#define USER_NOT_ON_CHANNEL ":localhost 441 " + client->GetNick() + " " + nick + " " + channel + " :They aren't on that channel\r\n"
#define JOIN_MSG ":" + client->GetNick() + "!" + client->GetUsername() + "@localhost JOIN :" + trimmed_channel_name + "\r\n"
#define INVITE_ONLY_ERROR ":localhost 473 " + client->GetNick() + " " + trimmed_channel_name + " :Cannot join channel (+i) - you must be invited\r\n"
#define INVITE_ALREADY_CONNECTED ":localhost 443 " + client->GetNick() + " " + trimmed_channel_name + " :is already on channel\r\n"
#define ALREADY_IN_CHANNEL_ERROR ":localhost 443 " + client->GetNick() + " " + trimmed_channel_name + " :is already on channel\r\n"
#define BAD_KEY_ERROR ":localhost 475 " + client->GetNick() + " " + trimmed_channel_name + " :Cannot join channel (+k) - bad key\r\n"
#define CHANNEL_FULL_ERROR ":localhost 471 " + client->GetNick() + " " + trimmed_channel_name + " :Cannot join channel (+l) - channel is full, try again later\r\n"
#define CHANNEL_NAME_ERROR ":localhost 403 * " + channel_name + " :Channel name must start with '#'\r\n"
#define RECEIVE_DEBUG "\033[31m<<\033[0m " << client->GetFd() << " " + channel + " " + command + " " + trimNewline(parameters)
#define NEW_CHANNEL_MSG ":localhost 353 " + client->getNickname() + " = " + channel->GetChannelName() + " :@" + client->getNickname() + "\r\n"
#define LIST_USER ":localhost 353 " + client->getNickname() + " = " + _channel_name + " :" + array_op + " " + array_user + "\r\n"
#define NOT_OP ":localhost 482 " + input->client->getNickname() + " " + getChannelByName(input->channel)->GetChannelName() + " :You're not a channel operator\r\n"
#define END_OF_NAMES_MSG ":localhost 366 " + client->getNickname() + " " + channel->GetChannelName() + " :End of /NAMES list.\r\n"
#define PRIV_MSG ":" + client->GetNick() + "!~" + client->GetUsername() + "@localhost PRIVMSG " + nickname + " :" + msg_content + "\r\n"
#define SEND_DEBUG "\033[32m>>\033[0m " << _fd << " " << trimNewline(msg)
#define MODE_MSG_NEW ":" + input->client->GetNick() + "!" + input->client->GetUsername() + "@localhost MODE " + channel->GetChannelName() + " "
#define LIMIT_MODE_ERROR ":" + channel->GetChannelName() + " 696 " + input->client->GetNick() + " " + channel->GetChannelName() + " l * :You must specify a parameter for the limit mode\r\n"
#define INVALID_LIMIT_VALUE_MSG ":localhost 461 " + input->client->GetNick() + " MODE :Invalid limit value\r\n"
#define NOT_ENOUGH_PARAMETERS_MSG ":localhost 461 " + input->client->GetNick() + " MODE :Not enough parameters for -k\r\n"
#define INVALID_PASSWORD_MSG ":localhost 464 " + input->client->GetNick() + " :Invalid password\r\n"
#define NOT_ENOUGH_PARAMETERS_FOR_O_MSG ":localhost 461 " + input->client->GetNick() + " MODE :Not enough parameters for -o\r\n"
#define QUIT_MSG ":" + client->GetNick() + "!~" + client->GetUsername() + "@localhost QUIT :Client Quit\r\n"
#define KEY_MODE_ERROR ":" + channel->GetChannelName() + " 696 " + input->client->GetNick() + " " + channel->GetChannelName() + " k * :You must specify a parameter for the key mode\r\n"
#define NO_SUCH_NICK_CHANNEL ":localhost 401 " + input->client->GetNick() + " " + username + " :No such nick/channel\r\n"
#define TOPIC_MSG ":localhost 332 " + input->client->GetNick() + " " + input->channel + " :" + chan->getTopic() + "\r\n"
#define OPERATOR_MODE_ERROR ":" + channel->GetChannelName() + " 696 " + input->client->GetNick() + " " + channel->GetChannelName() + " o * :You must specify a parameter for the operator mode\r\n"
#define PART_MSG ":" + input->client->GetNick() + "!~" + input->client->GetUsername() + "@localhost PART " + input->channel + "\r\n"
#define OPERATOR_ERROR "Impossible to add client to list of operators"
