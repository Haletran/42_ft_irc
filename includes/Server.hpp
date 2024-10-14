#pragma once

#include <iostream>

class Server {
    private:
        int _port;
        std::string _password;
        Server(const Server& cpy);
        Server operator=(const Server&src);
    public:
        Server(int port, std::string password);
        ~Server();
};
