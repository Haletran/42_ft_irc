#pragma once
#include "Global.hpp"

class Channel : public Server
{
	protected :
		std::string _channel_name;
		std::vector<Client*> _operators;
		std::vector<Client*> _clients;
		std::string _topic;
		bool _invite_only;
		std::string _password;
		bool password_needed;
		int _user_limit;
	public :
		// COPLIEN
		Channel();
		Channel(std::string channel_name);
		~Channel();
		bool operator==(const std::string &other) const;

		// FUNCTIONS
		std::string getTopic();
		void setTopic(std::string topic);
		const std::string &GetChannelName();
		void addOperators(Client *client);
		bool IsOP(Client *client);
		void setInviteOnly(bool invite_only);
		void setPassword(std::string password);
		void setUserLimit(int user_limit);
		void setPasswordNeeded(bool condition);
		void getAllUser(Client *client);
		bool getInvite();
		std::string getFlag();
		std::string getPassword();
		bool getPasswordNeeded();
		int getNbUser() const;
		int getlimit();

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
