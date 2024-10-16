#include <poll.h>
#include <netinet/in.h> // For sockaddr_in, inet_addr
#include <sys/socket.h> // For socket, bind, listen, accept
#include <unistd.h>     // For close
#include <iostream>
#include <cstring>      // For memset

#define PORT 8080
#define MAX_CLIENTS 10

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    // Prepare the sockaddr_in structure for the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(server_fd);
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return -1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // Set up an array of pollfd to monitor multiple sockets
    struct pollfd fds[MAX_CLIENTS + 1];  // +1 for the server socket
    memset(fds, 0, sizeof(fds));

    // Add the server socket to the poll list
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    // Initialize the rest of the pollfd array for clients
    for (int i = 1; i <= MAX_CLIENTS; ++i) {
        fds[i].fd = -1;  // -1 means this entry is not used
    }

    while (true) {
        // Call poll() to monitor all file descriptors
        int activity = poll(fds, MAX_CLIENTS + 1, -1);  // Block indefinitely

        if (activity < 0) {
            std::cerr << "Poll error" << std::endl;
            break;
        }

        // Check if there is activity on the server socket (new connection)
        if (fds[0].revents & POLLIN) {
            new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            if (new_socket < 0) {
                std::cerr << "Accept failed" << std::endl;
                break;
            }

            std::cout << "New connection, socket FD is " << new_socket << std::endl;

            // Add the new client socket to the poll list
            for (int i = 1; i <= MAX_CLIENTS; ++i) {
                if (fds[i].fd == -1) {  // Find an empty slot
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;  // Monitor for incoming data
                    std::cout << "Added to poll list at index " << i << std::endl;
                    break;
                }
            }
        }

        // Check if there's activity on any client sockets
        for (int i = 1; i <= MAX_CLIENTS; ++i) {
            if (fds[i].fd != -1 && fds[i].revents & POLLIN) {
                // Data is ready to read from client
                char buffer[1024] = {0};
                int valread = read(fds[i].fd, buffer, sizeof(buffer));
                if (valread == 0) {
                    // Client has disconnected
                    std::cout << "Client disconnected, socket FD " << fds[i].fd << std::endl;
                    close(fds[i].fd);
                    fds[i].fd = -1;  // Mark as unused
                } else {
                    // Print received data and send a response back
                    std::cout << "Received message: " << buffer << std::endl;
                    std::string response = "Message received\n";
                    send(fds[i].fd, response.c_str(), response.size(), 0);
                }
            }
        }
    }

    // Close the server socket
    close(server_fd);
    return 0;
}
