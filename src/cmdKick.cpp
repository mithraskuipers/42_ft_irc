#include "./../incs/Command.hpp"
#include "./../incs/cmdKick.hpp"
#include "./../incs/Replies.hpp"

// command KICK

cmdKick::cmdKick(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{
	
}

cmdKick::~cmdKick()
{
	
}

void cmdKick::execute(Client *client, std::vector<std::string> arguments)
{
	// std::cout << arguments[0] << "FIRST ARG EXE" << std::endl;
	// std::cout << arguments[1] << "SECOND ARG EXE" << std::endl;
	// std::cout << arguments[2] << "TURD ARG EXE" << std::endl;
	if (arguments.size() == 3)
		client->clientKick(arguments[0], arguments[1], arguments[2]);
	else
		client->clientKick(arguments[0], arguments[1], "");
}