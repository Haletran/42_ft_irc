#pragma once

#include <iostream>          // For input/output in C++
#include <cstring>           // For memset, etc.
#include <string>            // For std::string
#include <sys/types.h>       // Basic system data types
#include <sys/socket.h>      // For socket functions
#include <netinet/in.h>      // For sockaddr_in, htons, etc.
#include <arpa/inet.h>       // For inet_addr, inet_ntoa
#include <netdb.h>           // For gethostbyname, getaddrinfo, freeaddrinfo
#include <unistd.h>          // For close, etc.
#include <fcntl.h>           // For fcntl
#include <poll.h>            // For poll
#include <csignal>           // For signal and sigaction
#include <cerrno>            // For error handling
#include <sys/stat.h>        // For fstat
#include <sys/types.h>       // For lseek
#include <stdexcept>
#include <stdlib.h>
#include <unistd.h>


#define DEFAULT_PORT 6667

#include "Server.hpp"
#include "Client.hpp"
