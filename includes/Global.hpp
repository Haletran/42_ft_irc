#pragma once

#include <iostream>
#include <sstream>
#include <vector> //-> for vector
#include <map> //-> for map
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <csignal> //-> for signal()
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <limits>
#include <ctime>
#include "Client.hpp"
#include "Typedef.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Bot.hpp"
#include "../includes/Log.hpp"
#include <algorithm>

std::string trimNewline(const std::string &str);
bool parseMessage(const std::string &msg, std::string &command,
                  std::string &channel, std::string &parameters);
std::string parseChannelName(const std::string &line);
void clearVector(std::vector<Client*>& vec);

// colors
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"
