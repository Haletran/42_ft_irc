#pragma once

#include "Global.hpp"

typedef struct s_input {
    std::string command;
    std::string channel;
    Client *client;
    std::string parameters;
    std::string msg;
    std::string prefix;
    std::map<std::string, std::string> modes;
} t_input;
