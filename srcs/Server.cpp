#include "../includes/Server.hpp"

Server::Server(int port, std::string password) : _password(password)
{
    if (port <= 0)
        throw std::runtime_error("Port is not valid");
    std::cout << "Server is now running at port " << port << std::endl;
}


Server::~Server()
{
    std::cout << "Server is now down" << std::endl;
}


int Server::get_port()
{
    return (_port);
}
