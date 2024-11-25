#include "../includes/Global.hpp"

Bot::Bot(const std::string& ip, const std::string& name) {
    this->sockfd = -1;
    this->server_ip = ip;
    this->port = 6697;
    this->nickname = name;
    this->channel = "#bot";
    this->password = "mdp";
    this->isStarted = false;
    std::stringstream ss;
    std::time_t current_time = std::time(0);
    ss << current_time;
    this->time_created = ss.str();
}

Bot::~Bot() {
    disconnect();
    std::cout << "\033[1;31m" << nickname << " is now offline\033[0m" << std::endl;
}

bool Bot::send_message(const std::string& message) {
    std::string msg = message + "\r\n";
    if (!send(sockfd, msg.c_str(), msg.size(), 0))
        return (false);
    usleep(200000);
    return (true);
}

bool Bot::connect_to_server() {
    struct sockaddr_in server_addr;
    struct hostent* host;

    host = gethostbyname(server_ip.c_str());
    if (host == NULL) {
        std::cerr << "Error: Unable to resolve host " << server_ip << std::endl;
        return false;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error: Unable to create socket" << std::endl;
        return false;
    }

    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    std::memcpy(&server_addr.sin_addr.s_addr, host->h_addr, host->h_length);
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error: Unable to connect to server" << std::endl;
        return false;
    }
    return true;
}

void Bot::login() {
    send_message("PASS " + password);
    send_message("NICK " + nickname);
    send_message("USER " + nickname + " 0 * :" + nickname);
    send_message("JOIN " + channel);
    send_message("TOPIC " + channel + " :🎮 Welcome to the game Channel :) !");
    std::cout << "\033[1;32m" << nickname << " is now online\033[0m" << std::endl;
}

void Bot::printFile(std::string filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open script file" << std::endl;
        return;
    }
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        {
            lineCount++;
            if (!send_message("PRIVMSG " + channel + " :" + line)) {
                std::cerr << "Error: Unable to send message at line " << lineCount << std::endl;
                break;
            }
        }
    }
}

void Bot::renderVideo(std::string frames_directory) {
    DIR* dir;
    struct dirent* ent;
    std::vector<std::string> frames;

    send_message("PRIVMSG " + channel + " :ASCII animation starting in 3 seconds...");
    sleep(4);
    if (frames_directory == "includes/BotUtils/bad-apple")
        system("cvlc --play-and-exit includes/BotUtils/bad-apple/bad_apple.mp3 > /dev/null 2>&1 &");
    if ((dir = opendir(frames_directory.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string file_name = ent->d_name;
            if (file_name.find("out") == 0 && file_name.find(".txt") != std::string::npos) {
                frames.push_back(frames_directory + "/" + file_name);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error: Unable to open frames directory" << std::endl;
        return;
    }
    std::sort(frames.begin(), frames.end());


    for (std::vector<std::string>::iterator it = frames.begin(); it != frames.end(); ++it) {
        std::ifstream file(it->c_str());
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open frame file " << *it << std::endl;
            return ;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                if (!send_message("PRIVMSG " + channel + " :" + line)) {
                    std::cerr << "Error: Unable to send message from frame " << *it << std::endl;
                    return ;
                }
            }
        }
        file.close();
        usleep(100000);
    }
    send_message("PRIVMSG " + channel + " :ASCII animation finished");
}


void Bot::receive_messages() {
    char buffer[512];
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;

        std::string message(buffer);
        std::cout << message;

        if (message.find("!help") != std::string::npos)
            send_message("PRIVMSG " + channel + " :Here are the commands: !help, !ping, !ascii, !bad-apple");
        else if (message.find("!ping") != std::string::npos)
            send_message("PRIVMSG " + channel + " :" + nickname +  " is alive and was created at " + time_created);
        else if (message.find("!bad-apple") != std::string::npos)
            renderVideo("includes/BotUtils/bad-apple");
        else if (message.find("quoi") != std::string::npos)
        {
            switch(rand() % 2)
            {
                case(0):
                    send_message("PRIVMSG " + channel + " :feur");
                    break;
                case(1):
                    send_message("PRIVMSG " + channel + " :quoicoubeh");
                    break;
            }
        }
        else if (message.find("!ascii") != std::string::npos)
        {
            switch(rand() % 3)
            {
                case 0:
                    printFile("includes/BotUtils/bapasqui");
                    break;
                case 1:
                    printFile("includes/BotUtils/tim");
                    break;
                case 2:
                    printFile("includes/BotUtils/shrek");
                    break;
            }
        }
    }
}

void Bot::disconnect() {
    if (sockfd != -1) {
        send_message("PART " + channel + " :Leaving");
        close(sockfd);
        sockfd = -1;
    }
}

void Bot::SignalHandler(int signal) {
    (void)signal;
    throw std::runtime_error("Signal received");
}


int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <name>" << std::endl;
        return 1;
    }
    srand(time(0));
    std::string nickname = argv[2];
    std::string ip = argv[1];
    Bot client(ip, nickname);
    try {
        signal(SIGPIPE, SIG_IGN);
        signal(SIGINT, client.SignalHandler);
        if (!client.connect_to_server()) {
            return 1;
        }
        client.login();
        client.receive_messages();
    }
    catch (std::runtime_error &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        client.disconnect();
        return 1;
    }
    return 0;
}
