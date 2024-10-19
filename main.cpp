#include "Server.hpp"

int main()
{
	Server server;
	std::cout << "---- SERVER ----" << std::endl;
	try
	{
		signal(SIGINT, Server::SignalHandler);
		signal(SIGQUIT, Server::SignalHandler);
		server.ServerInit();
	}
	catch(const std::exception& e)
	{
		server.CloseFds();
		std::cerr << e.what() << '\n';
	}
	std::cout << "Server closed" << std::endl;
	return 0;
}