#include "../includes/Global.hpp"
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

void Server::ProcedeCommand(const std::string &msg, Client *client)
{
	std::istringstream stream(msg);
    std::string command, channel, parameters;
	if (!(stream >> command)) {
        std::cerr << "Invalid message format: No command found" << std::endl;
        return;
    }
	if (!(stream >> channel)) {
		std::cerr << "Invalid message format: No channel found" << std::endl;
		return;
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
	if(parameters.find_first_not_of(" \r\n") == std::string::npos)
	{
		parameters.clear();
	}  
	if (parameters[0] == ':') {
		parameters.erase(0, 1);
	}
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
	else if (command == "PRIVMSG")
	{
		std::string channel = parseChannelName(msg);
		std::string msg_content = msg.substr(msg.find(":", 1) + 1);
   		SendMessageToChannel(channel, client, msg_content);
	}
	else if (command == "TOPIC")
	{
		if (parameters.empty())
		{
			std::string currentTopic = getChannelByName(channel)->getTopic();
       		if (currentTopic.empty()) 
			{
            	std::string msg = ":localhost 331 " + client->GetNick() + " " + channel + " :No topic is set\r\n";
            	client->SendMsg(msg);
        	} 
			else 
			{
            	std::string msg = ":localhost 332 " + client->GetNick() + " " + channel + " :" + currentTopic + "\r\n";
            	client->SendMsg(msg);
        	}
		}
		else
		{
			getChannelByName(channel)->setTopic(parameters);
			std::string notificationMsg = ":" + client->GetNick() + "!" + client->GetUsername() + "@localhost TOPIC " + channel + " :" + parameters + "\r\n";
			for (std::vector<Client*>::iterator it = _channels[getChannelByName(channel)].begin(); it != _channels[getChannelByName(channel)].end(); ++it)
					(*it)->SendMsg(notificationMsg);
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

std::string trimNewline(const std::string &str) {
    size_t end = str.find_last_not_of("\r\n ");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

void Server::SendMessageToChannel(const std::string& channel_name, Client* sender, const std::string& message) 
{
    Channel* channel = getChannelByName(channel_name);
    if (channel == NULL) {
        std::string errorMsg = "Channel " + channel_name + " does not exist.\r\n";
        sender->SendMsg(errorMsg);
        return;
    }
    std::string formatted_message = ":" + sender->GetNick() + "!" + sender->GetUsername() + "@localhost PRIVMSG " + channel_name + " :" + message + "\r\n";
    for (std::vector<Client*>::iterator it = _channels[channel].begin(); it != _channels[channel].end(); ++it) {
        if (*it != sender) {
            (*it)->SendMsg(formatted_message);
        }
    }
}

void Server::KickFromChannel(const std::string &channel, const std::string &nickname, Client *client)
{
    std::string nick = trimNewline(nickname);
    Channel* channelPtr = getChannelByName(channel);
    if (channelPtr == NULL) 
    {
        std::string errorMsg = "403 " + client->GetNick() + " " + channel + " :No such channel\r\n";
        client->SendMsg(errorMsg);
        return;
    }
    std::vector<Client*> &clientsInChannel = _channels[channelPtr];
    for (std::vector<Client*>::iterator clientIt = clientsInChannel.begin(); clientIt != clientsInChannel.end(); ++clientIt) {
        if ((*clientIt)->GetNick() == nick) {
            std::string kickMsg = ":" + client->GetNick() + " KICK " + channel + " " + nick + " :Kicked by " + client->GetNick() + "\r\n";
            for (std::vector<Client*>::iterator notifyIt = clientsInChannel.begin(); notifyIt != clientsInChannel.end(); ++notifyIt) 
            {
                (*notifyIt)->SendMsg(kickMsg);
            }
            LeaveChannel(channel, *clientIt);
            return;
        }
    }
    std::string errorMsg = ":localhost 441 " + client->GetNick() + " " + nick + " " + channel + " :They aren't on that channel\r\n";
    client->SendMsg(errorMsg);
}

