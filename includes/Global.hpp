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
#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "../includes/Log.hpp"


// colors
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"
