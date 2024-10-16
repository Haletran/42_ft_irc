#include "../includes/Global.hpp"
#include <vector>

// Source :
// https://reactive.so/post/42-a-comprehensive-guide-to-ft_irc/
// https://www.keypuncher.net/blog/network-sockets-in-c
int main(int argc, char **argv)
{
    if (argc <= 2)
    {
        std::cerr << "Not enough or too much paramaters" << std::endl;
        return (1);
    }

    try {
        std::string password = argv[2];
        int port = static_cast<int>(std::strtod(argv[1], NULL));
        int server_fd = 0;
        int new_socket;
        Server instance(port ,password , server_fd);

        struct pollfd fds[MAX_CLIENTS + 1];
        memset(fds, 0, sizeof(fds));
        fds[0].fd = instance.get_fd();
        fds[0].events = POLLIN;
        for (int i = 1; i <= MAX_CLIENTS; ++i) {
            fds[i].fd = -1;
        }

        while (true) {
            // Call poll() to monitor all file descriptors
            int activity = poll(fds, MAX_CLIENTS + 1, -1);
            if (activity < 0) {
                std::cerr << "Poll error" << std::endl;
                break;
            }
            // CHECK if user connect to the server
            if (fds[0].revents & POLLIN) {
                struct sockaddr_in address = instance.get_address();
                socklen_t addrlen = sizeof(address);
                new_socket = accept(instance.get_fd(), (struct sockaddr*)&address, &addrlen);
                if (new_socket < 0) {
                    std::cerr << "Accept failed" << std::endl;
                    break;
                }
                std::cout << "New connection, socket FD is " << new_socket << std::endl;
                for (int i = 1; i <= MAX_CLIENTS; ++i) {
                    if (fds[i].fd == -1) {
                        fds[i].fd = new_socket;
                        fds[i].events = POLLIN;
                        std::cout << "Added to poll list at index " << i << std::endl;
                        std::string preAuthMessage = "NOTICE AUTH :*** Welcome to the IRC server! Please authenticate with PASS, NICK, and USER commands.\r\n";
                        send(new_socket, preAuthMessage.c_str(), preAuthMessage.length(), 0);
                        break;
                    }
                }
            }

            // handle message and deconnection
            std::vector<char> buffer(1024);
            for (int i = 1; i <= MAX_CLIENTS; ++i) {
                if (fds[i].fd != -1 && fds[i].revents & POLLIN) {
                    int valread = read(fds[i].fd, buffer.data(), buffer.size() - 1);
                    if (valread == 0) {
                        std::cout << "Client disconnected, socket FD " << fds[i].fd << std::endl;
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    } else {
                        std::cout << buffer.data() << std::endl;
                        instance.login(buffer, new_socket);
                        buffer.clear();
                        buffer.resize(1024);
                    }
                }
            }
        }
        close(instance.get_fd());
    } catch (std::runtime_error &e) {
        std::cerr << "Error : " << e.what() << std::endl;
    }
    return (0);
}
