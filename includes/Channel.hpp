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
		std::string getTopic();
		void setTopic(std::string topic);
		bool operator==(const std::string &other) const;
		class ChannelException : public std::exception
		{
			public :
				ChannelException(const std::string &msg) : _msg(msg) {}
				virtual const char *what() const throw()
				{
					return _msg.c_str();
				}
				~ChannelException() throw() {}
			private :
				std::string _msg;
		};
};