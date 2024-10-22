#include "../includes/Global.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./server <port> <password>" << std::endl;
		return 1;
	}
	Server server;
	std::cout << "---- SERVER ----" << std::endl;
	try
	{
		signal(SIGINT, Server::SignalHandler);
		signal(SIGQUIT, Server::SignalHandler);
		server.ServerInit(atoi(argv[1]), argv[2]);
	}
	catch(const std::exception& e)
	{
		server.CloseFds();
		std::cerr << e.what() << '\n';
	}
	std::cout << "Server closed" << std::endl;
	return 0;
}
