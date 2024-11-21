#pragma once
#include "Global.hpp"

class Channel : public Server
{
protected:
	std::string _channel_name;
	std::vector<Client *> _operators;
	std::vector<Client *> _clients;
	std::vector<Client *> _invited_clients;
	std::string time_created;
	std::string _topic;
	bool _invite_only;
	bool _topic_change;
	std::string _password;
	bool password_needed;
	int _user_limit;

public:
	// COPLIEN
	Channel();
	Channel(std::string channel_name);
	~Channel();
	bool operator==(const std::string &other) const;

	// Getters
	std::string getTopic();
	const std::string &GetChannelName();
	std::string getTimeCreated();
	bool getInvite();
	bool getTopicChange();
	std::string getFlag();
	std::string getPassword();
	bool getPasswordNeeded();
	int getNbUser() const;
	int getlimit();
	std::string getChannelName();

	// Setters
	void setTopic(std::string topic);
	void setInviteOnly(bool invite_only);
	void setTopicChange(bool value);
	void setPassword(std::string password);
	void setUserLimit(int user_limit);
	void setPasswordNeeded(bool condition);

	// Operations
	void addOperators(Client *client);
	void addInvited(Client *);
	bool IsOP(Client *client);
	bool IsInvited(Client *);
	void getAllUser(Client *client);
	bool isAlreadyConnected(Client *client);
	void removeOperator(Client *client);
	void CleanChannel(Channel *chan);
	void removeClient(Client *client);

	class ChannelException : public std::exception
	{
	public:
		ChannelException(const std::string &msg) : _msg(msg) {}
		virtual const char *what() const throw()
		{
			return _msg.c_str();
		}
		~ChannelException() throw() {}

	private:
		std::string _msg;
	};
};
