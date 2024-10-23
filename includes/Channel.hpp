#pragma once 
#include "Global.hpp"

class Channel : public Server
{
	protected :
		std::string _channel_name;
		std::vector<Client*> _clients;
		std::string _topic;
		bool _invite_only;
	public :
		//std::string _channel_name;
		Channel();
		Channel(std::string channel_name);
		~Channel();
		const std::string &GetChannelName();
		bool operator==(const std::string &other) const;
};