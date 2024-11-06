#include "../includes/Global.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./server <port> <password>" << std::endl;
		return 1;
	}
	Server server;
	std::cout << "\033[1;34m---- SERVER ----\033[0m" << std::endl;
	try
	{
		signal(SIGINT, Server::SignalHandler);
		// need to fix CTRL+D not working properly
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
