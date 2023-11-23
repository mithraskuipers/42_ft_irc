/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cmdJoin.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 20:50:13 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:11:52 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Command.hpp"
#include "./../incs/cmdJoin.hpp"
#include "./../incs/Replies.hpp"

cmdJoin::cmdJoin(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{
	
}

cmdJoin::~cmdJoin()
{
	
}

// format : JOIN <channel>{,<channel>} [<key>{,<key>}]

void cmdJoin::execute(Client *client, std::vector<std::string> arguments)
{

	if (arguments.empty())
	{
		client->sendMessageToClientItself(ERR_NEEDMOREPARAMS(client->getNickname(), "PASS"));
		return ;
	}

	std::string name = arguments[0];
	std::string password = arguments.size() > 1 ? arguments[1] : "";

	Channel *channel = client->getChannelInstance();
	if (channel)
	{
		client->sendMessageToClientItself(ERR_TOOMANYCHANNELS(client->getNickname(), name));
		return ;
	}

	channel = server->getChannelInstance(name);
	if (!channel)
		channel = server->createChannel(name, password, client);

	if (channel->getMaxClients() > 0 && channel->getNumClients() >= channel->getMaxClients())
	{
		client->sendMessageToClientItself(ERR_CHANNELISFULL(client->getNickname(), name));
		return ;
	}

	if (channel->getPassword() != password)
	{
		client->sendMessageToClientItself(ERR_BADCHANNELKEY(client->getNickname(), name));
		return ;
	}
	if (channel->channelGetInviteOnly() == true && channel->checkInvited(client) == false)
	{
		client->sendMessageToClientItself(ERR_INVITEONLYCHAN(client->getNickname(), name));
		return ;
	}
	client->clientJoinChannel(channel);
}