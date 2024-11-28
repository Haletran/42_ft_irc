#include "../includes/Global.hpp"
#include <csignal>

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
		signal(SIGQUIT, Server::SignalHandler);
		signal(SIGPIPE, SIG_IGN);
		server.ServerInit(atoi(argv[1]), argv[2]);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	std::cout << "\033[1;31mServer down\033[0m" << std::endl;
	return 0;
}
