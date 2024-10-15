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
        Server instance(port ,password);
        int server_fd;

        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1)
            throw std::runtime_error("Error creating socket");


        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port); // Port number
        serverAddress.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface

        if (bind(server_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Error binding socket" << std::endl;
            close(server_fd);
            return 1;
        }

        // Listen for connections
            if (listen(server_fd, 5) == -1) {
                std::cerr << "Error listening" << std::endl;
                close(server_fd);
                return 1;
            }

            // Accept connections
            sockaddr_in clientAddress;
            socklen_t clientSize = sizeof(clientAddress);
            int clientSocket = accept(server_fd, (struct sockaddr *)&clientAddress, &clientSize);
            if (clientSocket == -1) {
                std::cerr << "Error accepting connection" << std::endl;
                close(server_fd);
                return 1;
            }

            // get connection
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
            int clientPort = ntohs(clientAddress.sin_port);
            std::cout << "Accepted connection from " << clientIP << ":" << clientPort
                      << std::endl;

            // Send data to the client
            const char *message = "Hello from server!\n";
            send(clientSocket, message, strlen(message), 0);

            // Received data from the client
            std::vector<char> msg_buffer(1024);
            while (1)
            {
                ssize_t bytes_read = read(clientSocket, msg_buffer.data(), msg_buffer.size() - 1);
                if (bytes_read < 0) {
                    close(clientSocket);
                    close(server_fd);
                    return 1;
                }
                if (bytes_read == 0)
                    break;
                instance.login(msg_buffer);
                msg_buffer.clear();
                msg_buffer.resize(1024);
            }

            // Close sockets
            close(clientSocket);
    } catch (std::runtime_error &e) {
        std::cerr << "Error : " << e.what() << std::endl;
    }
    return (0);
}
