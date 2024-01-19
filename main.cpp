#include "server/Server.hpp"

int main(int ac, char **av)
{
	try
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		if ((atoi(av[1]) < 1024) || (atoi(av[1]) > 65535))
			throw std::runtime_error("invalid port, to be value to be 1024 up to 65535 (6667 for default)");
		Server server(av[1], av[2]);
		server.runServer();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
