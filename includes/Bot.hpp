#pragma once

#include "Global.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <vector>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <ctime>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <csignal>
#include <fstream>

class Bot {
    private:
        int sockfd;
        std::string server_ip;
        int port;
        std::string nickname;
        std::string channel;
        std::string password;
        bool send_message(const std::string& message);
        bool isStarted;
    public:
        Bot(const std::string& ip, const std::string& name);
        ~Bot();
        bool connect_to_server();
        void login();
        void join_channel();
        void receive_messages();
        void disconnect();
        void printFile(std::string filename);
        void sendFormattedBlock(const std::string& message);
        static void SignalHandler(int signum);
        void renderVideo(std::string frames_directory);
};
