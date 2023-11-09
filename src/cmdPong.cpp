#include "./../incs/Command.hpp"
#include "./../incs/cmdPong.hpp"
#include "./../incs/Replies.hpp"


cmdPong::cmdPong(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{

}

cmdPong::~cmdPong()
{

}

void cmdPong::run(Client *client, std::vector<std::string> arguments)
{

	if (arguments.empty())
	{
		client->sendMessageToClientItself(ERR_NEEDMOREPARAMS(client->getNickname(), "PONG"));
		return ;
	}

	client->sendMessageToClientItself(RPL_PING(client->getPrefix(), arguments.at(0)));
}