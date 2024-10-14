#pragma once

#include "Global.hpp"

class Client {
    private:
        std::string _username;
        std::string _nickname;
        Client (const Client& cpy);
        Client operator=(const Client& src);
    public:
        Client(std::string username, std::string nickname);
        ~Client();
};
