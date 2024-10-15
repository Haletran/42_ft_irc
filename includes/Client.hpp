#pragma once

#include <iostream>

class Client {
    private:
        std::string _username;
        std::string _nickname;
        int _state;
    public:
        Client(int _state);
        Client (const Client& cpy);
        Client &operator=(const Client& src);
        ~Client();

        // SETTERS
        void set_username(std::string username);
        void set_state(int state);
        void set_nickname(std::string nickname);

        // GETTERS
        std::string get_username();
        int get_state();
        std::string get_nickname();
};
