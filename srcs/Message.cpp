#include "../includes/Global.hpp"
#include <sstream>

void Server::ProcedeCommand(const std::string &msg, Client *client)
{
	std::string::size_type i = msg.find(" ");
    if (i == std::string::npos) {
        std::cerr << "Invalid message format" << std::endl;
        return;
    }
    std::string command = msg.substr(0, i);
    std::string rest = msg.substr(i + 1);
    std::string::size_type j = rest.find(" ");
    std::string channel = rest.substr(0, j);
    std::string parameters = rest.substr(j + 1);
    std::cout << "Command: " << command << " Channel: " << channel << " Parameters: " << parameters << std::endl;
    if (command == "JOIN") {
        if (channel[channel.length() - 1] == '\n')
            channel = channel.substr(0, channel.length() - 1);
        JoinChannel(channel, client);
    }
    else if (command == "INVITE")
    {
        std::string msg = ":" + client->GetUsername() + " INVITE " + channel + " " + parameters;
        Client *test = get_ClientByUsername(channel);
        if (!test)
        {
            std::string error = ": 401 " + client->GetNick() + " " + channel + " :No such nick\n";
            client->SendMsg(error);
        }
        else
        {
            std::string msg3 = ": " + client->GetUsername() + " INVITE " + test->GetUsername() + " :#" + parameters + "\n";
            client->SendMsg(msg3);
            std::string msg4 = ": 341 " + client->GetUsername() + " " + test->GetUsername() + " #" + parameters + "\n";
            JoinChannel(parameters, test);
        }
    }
	else if (command == "KICK")
	{
		KickFromChannel(channel, parameters, client);
	}
	else
	{
		std::string msg = "Unknown command\n";
		client->SendMsg(msg);
	}

/*
	else if (msg.find("LEAVE") != std::string::npos)
	{
		std::string channel = msg.substr(6);
		if (channel[channel.length() - 1] == '\n')
			channel = channel.substr(0, channel.length() - 1);
		LeaveChannel(channel, client);
	}
	else if (msg.find("WHOAMI") != std::string::npos)
	{
		std::string msg = "Nick: " + client->GetNick() + "\n";
		client->SendMsg(msg);
		msg = "Username: " + client->GetUsername() + "\n";
		client->SendMsg(msg);
	}
	else if (msg.find("KICK") != std::string::npos)
	{
		KickFromChannel(msg, client);
	}
	*/
}

std::string GetChannelName(const std::string &line) {
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

void Server::ProcedeChannelMessage(const std::string &msg, Client *client)
{
	//PRIVMSG dd :slt
	std::string formated_message = ":" + client->GetUsername() + " " + msg;
	std::string channel = GetChannelName(msg);
    SendMessageToChannel(channel, formated_message, client);
    //AddMessageToChannel(channel, formatted_msg);
}

void Server::ProcedeMessage(const std::string &msg, Client *client)
{
	std::string channel;
	std::string message;
	std::istringstream stream(msg);
	std::string line;
	while (std::getline(stream, line))
	{
		if (line.find("PRIVMSG ") == std::string::npos)
			ProcedeCommand(line, client);
		else
			ProcedeChannelMessage(line, client);
	}
}

std::string trimNewline(const std::string &str) {
    size_t end = str.find_last_not_of("\r\n ");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

void Server::SendMessageToChannel(const std::string &channelName, const std::string &msg, Client *client)
{
    std::string trimmedChannelName = trimNewline(channelName);
    std::map<std::string, std::vector<Client*> >::iterator it = _channels.find(trimmedChannelName);
    if (it != _channels.end())
    {
        std::string formatted_msg = ":" + client->GetUsername() + "!" + client->GetNick() + " PRIVMSG " + trimmedChannelName + " :" + msg + "\r\n";
        std::vector<Client*> clientsInChannel = GetClientsFromChannel(channelName);
		for (std::vector<Client*>::iterator clientIt = clientsInChannel.begin(); clientIt != clientsInChannel.end(); ++clientIt)
		{
			std::cout << "Sending message to client: " << (*clientIt)->GetUsername() << std::endl;
			if (*clientIt != client)
			{
				(*clientIt)->SendMsg(msg);
			}
		}
    }
    else
    {
        std::string error_msg = "Channel does not exist\n";
        client->SendMsg(error_msg);
    }
}

void Server::AddMessageToChannel(const std::string &channel, const std::string &msg)
{
	std::map<std::string, std::vector<Client*> >::iterator it;

	if (_channels.find(channel) != _channels.end()) {

        _channelMessages[channel].push_back(msg);
    } else {
        std::cerr << "Channel "<< channel <<" does not exist" << std::endl;
    }

}

std::vector<std::string> Server::GetMessagesFromChannel(const std::string &channel)
{
    if (_channels.find(channel) != _channels.end()) {
        return _channelMessages[channel];
    } else {
        std::cerr << "Channel does not exist" << std::endl;
        return std::vector<std::string>();
    }
}

void Server::KickFromChannel(const std::string &nick, const std::string &channel, Client *client)
{
	std::string trimmed_channel_name = trimNewline(channel);
	std::map<std::string, std::vector<Client*> >::iterator it = _channels.find(trimmed_channel_name);
	if (it != _channels.end())
	{
		std::vector<Client*> clientsInChannel = GetClientsFromChannel(trimmed_channel_name);
		for (std::vector<Client*>::iterator clientIt = clientsInChannel.begin(); clientIt != clientsInChannel.end(); ++clientIt)
		{
			if ((*clientIt)->GetNick() == nick)
			{
				std::string msg = "You have been kicked from channel " + trimmed_channel_name + "\n";
				(*clientIt)->SendMsg(msg);
				LeaveChannel(trimmed_channel_name, *clientIt);
				break;
			}
		}
	}
	else
	{
		std::string msg = "Channel does not exist\n";
		client->SendMsg(msg);
	}
}
