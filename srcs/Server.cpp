#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <sstream>


Server::Server(int port, std::string password) : _password(password)
{
    if (port <= 0)
    {
        std::cerr << "Error : Port is not valid" << std::endl;
        std::cerr << "Setting the default port" << std::endl;
        _port = 6667;
    }
    std::cout << "Server is now running at port " << _port << std::endl;
}


Server::~Server()
{
    std::cout << "Server is now down" << std::endl;
}

void Server::login(std::vector<char> msgBuffer)
{
    std::string cmd_array[3] = {"PASS", "USER", "NICK"};
    bool found = false;
    std::stringstream ss(msgBuffer.data());
    std::string conv;
    int i = 0;
    std::string input;
    std::string cmd;
    while(ss >> conv)
    {
        if (i == 1)
            input = conv;
        if (i == 0)
            cmd = conv;
        i++;
    }
    for (int i = 0; i < 3; i++)
    {
        if (cmd == cmd_array[i])
        {
            found = true;
            switch (i) {
                case 0:
                {
                    if (listUser.empty())
                    {
                        if (!input.empty())
                        {
                            if (input == _password)
                            {
                                Client new_client(0);
                                listUser.push_back(new_client);
                                std::cout << "Password accepted, client added" << std::endl;
                            }
                            else
                            std::cout << "Invalid password" << std::endl;
                        }
                        else
                            std::cout << "Password not provided" << std::endl;
                    }
                    else
                        std::cout << "User already connected" << std::endl;
                    break;
                }
                case 1:
                    if (listUser.back().get_state() == 0 && !input.empty())
                    {
                        listUser.back().set_username(input);
                        listUser.back().set_state(1);
                    }
                    break;
                case 2:
                    if (listUser.back().get_state() == 1 && !input.empty())
                    {
                        listUser.back().set_state(2);
                        listUser.back().set_nickname(input);
                        // join default channel
                        std::cout << "User " << listUser.back().get_username() <<  " created successfully" << std::endl;
                    }
                    break;
            }
            break;
        }
    }
    if (!found)
        std::cout << "UNKNOWN COMMAND" << std::endl;
}


Client Server::get_client()
{
    if (listUser.empty())
        return Client(0);
    return listUser.back();
}


int Server::get_port()
{
    return (_port);
}
