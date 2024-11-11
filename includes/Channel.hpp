#pragma once
#include "Global.hpp"

class Channel : public Server
{
	protected :
		std::string _channel_name;
		std::vector<Client*> _operators;
		std::vector<Client*> _clients;
		std::vector<Client*> _invited_clients;
		std::string time_created;
		std::string _topic;
		bool _invite_only;
		bool _topic_change;
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
		void addInvited(Client*);
		bool IsOP(Client *client);
		bool IsInvited(Client*);
		void setInviteOnly(bool invite_only);
		void setTopicChange(bool value);
		void setPassword(std::string password);
		void setUserLimit(int user_limit);
		void setPasswordNeeded(bool condition);
		void getAllUser(Client *client);
		bool isAlreadyConnected(Client *client);
		std::string getTimeCreated();
		bool getInvite();
		bool getTopicChange();
		std::string getFlag();
		std::string getPassword();
		bool getPasswordNeeded();
		int getNbUser() const;
		int getlimit();
		void removeOperator(Client *client);
		void CleanChannel();

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
