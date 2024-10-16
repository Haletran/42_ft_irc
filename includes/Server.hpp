#pragma once

#include "Client.hpp"
#include "Global.hpp"
#include <iostream>
#include <vector>
#include <list>

class Server {
    private:
        int _port;
        int _server_fd;
        std::string _password;
        std::list<Client> listUser;
        sockaddr_in _serverAddress;
        Server(const Server& cpy);
        Server operator=(const Server&src);
    public:
        Server(int port, std::string password,int server_fd);
        ~Server();

        int get_port();
        void login(std::vector<char> msgBuffer, int client_socket);
        bool command_parsing();
        Client get_client();
        int get_fd();
        sockaddr_in get_address();
};
