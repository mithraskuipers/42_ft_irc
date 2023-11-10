/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cmdPart.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/10 08:17:23 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:17:24 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Command.hpp"
#include "./../incs/cmdPart.hpp"
#include "./../incs/Replies.hpp"

cmdPart::cmdPart(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{

}

cmdPart::~cmdPart()
{

}

// format: PART <channel>{,<channel>} [<reason>]
// Template: PART <channel
void cmdPart::execute(Client *client, std::vector<std::string> arguments)
{

	if (arguments.empty())
	{
		client->sendMessageToClientItself(ERR_NEEDMOREPARAMS(client->getNickname(), "PART"));
		return ;
	}

	std::string name = arguments[0];

	Channel *channel = server->getChannelInstance(name);
	if (!channel)
	{
		client->sendMessageToClientItself(ERR_NOSUCHCHANNEL(client->getNickname(), name));
		return ;
	}

	if (!client->getChannelInstance() || client->getChannelInstance()->getChannelName() != name)
	{
		client->sendMessageToClientItself(ERR_NOTONCHANNEL(client->getNickname(), name));
		return ;
	}

	client->clientLeaveChannel();
}