#include "../includes/Global.hpp"

bool Bot::send_message(const std::string& message) {
    std::string msg = message + "\r\n";
    return send(sockfd, msg.c_str(), msg.size(), 0) >= 0;
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
    // Send NICK and USER commands to log in to the server
    send_message("PASS " + password);
    send_message("NICK " + nickname);
    send_message("USER " + nickname + " 0 * :" + nickname);
    sleep(1);
    send_message("JOIN " + channel);
}

void Bot::receive_messages() {
    char buffer[512];
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;

        std::string message(buffer);
        std::cout << message;

        if (message.find("PING") != std::string::npos) {
            send_message("PRIVMSG " + channel + " :PONG");
        }
    }
}

void Bot::disconnect() {
    if (sockfd != -1) {
        send_message("QUIT :Client exiting");
        close(sockfd);
        sockfd = -1;
    }
}

Bot::~Bot() {
    disconnect();
}

int main() {
    std::string server = "localhost";
    int port = 6666;
    std::string nickname = "BOT";
    std::string channel = "#game";
    std::string password = "testtest2";

    Bot client(server, port, nickname, channel, password);
    if (!client.connect_to_server()) {
        return 1;
    }
    client.login();
    client.receive_messages();
    return 0;
}
