#pragma once

#include "Client.hpp"
#include <iostream>
#include <vector>
#include <list>

class Server {
    private:
        int _port;
        std::string _password;
        std::list<Client> listUser;
        Server(const Server& cpy);
        Server operator=(const Server&src);
    public:
        Server(int port, std::string password);
        ~Server();

        int get_port();
        void login(std::vector<char> msgBuffer);
        bool command_parsing();
};
