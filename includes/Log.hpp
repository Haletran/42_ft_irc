#pragma once

// i : set the channel to invite only
//nickname!~username@IP MODE #channel +i
//:bapasqui!~bapasqui@localhost MODE #asd : +i
// k <key> : set a channel key (password)
// l <limit> : set the user limit
// t : set the channel topic

#define MODE_MESSAGE ":" + client->GetNick() + "!~" + client->GetUsername() + "@localhost MODE " + channel + " " + parameters + "\r\n"
#define FLAG_MSG ":localhost " + client->getNickname() + channel + " +" + flag + "\r\n"
#define SET_TOPIC ":" + client->GetNick() + "!" + client->GetUsername() + "@localhost TOPIC " + channel + " :" + parameters + "\r\n"
#define EMPTY_TOPIC ":localhost 331 " + client->GetNick() + " " + channel + " :No topic is set\r\n"
#define TOPIC_ERROR ":localhost 332 " + client->GetNick() + " " + channel + " :" + currentTopic + "\r\n"
#define INVITE_MSG ":" + client->GetUsername() + " INVITE " + channel + " " + parameters
#define INVITE_USER_ERROR ": 401 " + client->GetNick() + " " + channel + " :No such nick\n"
#define INVITE_SUCCESS_MSG ": " + client->GetUsername() + " INVITE " + test->GetUsername() + " :#" + parameters + "\n"
#define INVITE_CONFIRM_MSG ": 341 " + client->GetUsername() + " " + test->GetUsername() + " #" + parameters + "\n"
#define FORMATTED_MESSAGE ":" + sender->GetNick() + "!" + sender->GetUsername() + "@localhost PRIVMSG " + channel_name + " :" + message + "\r\n"
#define CHANNEL_NOT_FOUND "403 " + client->GetNick() + " " + channel + " :No such channel\r\n"
#define KICK_MSG ":" + client->GetNick() + " KICK " + channel + " " + nick + " :Kicked by " + client->GetNick() + "\r\n"
#define USER_NOT_ON_CHANNEL ":localhost 441 " + client->GetNick() + " " + nick + " " + channel + " :They aren't on that channel\r\n"
#define JOIN_MSG ":" + client->GetNick() + "!" + client->GetUsername() + "@localhost JOIN :" + trimmed_channel_name + "\r\n"
#define INVITE_ONLY_ERROR ":localhost 473 " + client->GetNick() + " " + trimmed_channel_name + " :Cannot join channel (+i) - you must be invited\r\n"
#define BAD_KEY_ERROR ":localhost 475 " + client->GetNick() + " " + trimmed_channel_name + " :Cannot join channel (+k) - bad key\r\n"
#define CHANNEL_FULL_ERROR ":localhost 471 " + client->GetNick() + " " + trimmed_channel_name + " :Cannot join channel (+l) - channel is full, try again later\r\n"
#define CHANNEL_NAME_ERROR ":localhost 403 * " + channel_name + " :Channel name must start with '#'\r\n"
#define RECEIVE_DEBUG "\033[31m<<\033[0m " << client->GetFd() << " # " + channel + " " + command + " " + trimNewline(parameters)
#define SEND_DEBUG "\033[32m>>\033[0m " << _fd << trimNewline(msg)
