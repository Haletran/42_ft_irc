#pragma once

#include "Global.hpp"

typedef struct s_input {
    std::string command;
    std::string channel;
    Client *client;
    std::string parameters;
    std::string msg;
} t_input;
