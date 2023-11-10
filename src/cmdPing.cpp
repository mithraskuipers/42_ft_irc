#include "./../incs/Command.hpp"
#include "./../incs/cmdPing.hpp"
#include "./../incs/Replies.hpp"

cmdPing::cmdPing(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{

}

cmdPing::~cmdPing()
{

}

void cmdPing::execute(Client *client, std::vector<std::string> arguments)
{

	if (arguments.empty())
	{
		client->sendMessageToClientItself(ERR_NEEDMOREPARAMS(client->getNickname(), "PING"));
		return ;
	}

	client->sendMessageToClientItself(RPL_PING(client->getPrefix(), arguments.at(0)));
}