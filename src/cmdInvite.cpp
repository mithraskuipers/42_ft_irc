#include "./../incs/Command.hpp"
#include "./../incs/cmdInvite.hpp"
#include "./../incs/Replies.hpp"

// command INVITE 
// parameters <nickname> <channel>
// if the channel exists (doesn't have to), only channel members are allowed to invite
// if channel invite-only is set, only channel operators may issue the command
// only the inviting and invitee will receive notification

// cmd example INVITE invitee #CHANNEL
// msg to invitee inviter!mail@name.mail INVITE inviteeName #CHANNEL

cmdInvite::cmdInvite(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{
	
}

cmdInvite::~cmdInvite()
{
	
}

void cmdInvite::execute(Client *client, std::vector<std::string> arguments)
{
	// std::cout << arguments[0] << "FIRST ARG EXE" << std::endl;
	// std::cout << arguments[1] << "SECOND ARG EXE" << std::endl;

	// arguments.at(0) == target
	// arguments.at(1) == channel

	// if (arguments.empty())
	// {
	// 	client->sendMessageToClientItself(ERR_NEEDMOREPARAMS(client->getNickname(), "PASS"));
	// 	return ;
	// }

	// std::string name = arguments[0];
	// std::string password = arguments.size() > 1 ? arguments[1] : "";

	// Channel *channel = client->getChannelInstance();
	// if (channel)
	// {
	// 	client->sendMessageToClientItself(ERR_TOOMANYCHANNELS(client->getNickname(), name));
	// 	return ;
	// }

	// channel = server->getChannelInstance(name);
	// if (!channel)
	// 	channel = server->createChannel(name, password, client);

	// if (channel->getMaxClients() > 0 && channel->getNumClients() >= channel->getMaxClients())
	// {
	// 	client->sendMessageToClientItself(ERR_CHANNELISFULL(client->getNickname(), name));
	// 	return ;
	// }

	// if (channel->getPassword() != password)
	// {
	// 	client->sendMessageToClientItself(ERR_BADCHANNELKEY(client->getNickname(), name));
	// 	return ;
	// }

	client->clientInvite(arguments[0], arguments[1]);
}