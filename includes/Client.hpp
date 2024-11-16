#pragma once
#include "Global.hpp"

class Client
{
	private :
		int _fd;
		std::string _ip;
		bool _auth;
		std::string _nick;
		std::string _username;
		std::string _buffer;
		static const size_t MAX_BUFFER_SIZE = 10000;
	public :
		// Constructors
		Client();
		Client(Client const &src);	
		// Destructor
		~Client();	
		// Getters
		int GetFd();
		std::string GetNick();
		std::string GetUsername();
		bool GetAuth();
		std::string& GetBuffer();	
		size_t GetBufferSize() const;
		// Setters
		void SetFd(int fd);
		void SetIp(std::string ip);
		void SetNick(std::string nick);
		void SetUsername(std::string username);
		void SetAuth(bool auth);

	   // Other member functions
		void SendMsg(const std::string &msg);
		void AppendToBuffer(const std::string& data);
		std::string getNickname();
		void ClearBuffer();
};
