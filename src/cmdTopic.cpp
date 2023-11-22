#include "./../incs/Command.hpp"
#include "./../incs/cmdTopic.hpp"
#include "./../incs/Replies.hpp"

// command Topic 

cmdTopic::cmdTopic(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{
	
}

cmdTopic::~cmdTopic()
{
	
}

void cmdTopic::execute(Client *client, std::vector<std::string> arguments)
{
	std::cout << arguments[0] << "FIRST ARG EXE" << std::endl;
	std::cout << arguments[1] << "SECOND ARG EXE" << std::endl;
	std::cout << arguments[2] << "THIRD ARG EXE" << std::endl;
	// std::cout << arguments[3] << "FOURTHRST ARG EXE" << std::endl;

	client->clientSetTopic(arguments[0], arguments[2]);
}