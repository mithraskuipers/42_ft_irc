/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cmdPass.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 20:42:42 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:11:52 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Command.hpp"
#include "./../incs/cmdPass.hpp"
#include "./../incs/Replies.hpp"

cmdPass::cmdPass(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{
}

cmdPass::~cmdPass()
{
}

void cmdPass::execute(Client *client, std::vector<std::string> arguments)
{
	if (client->isRegistered())
	{
		client->sendMessageToClientItself(ERR_ALREADYREGISTERED(client->getNickname()));
		return ;
	}

	if (arguments.empty())
	{
		client->sendMessageToClientItself(ERR_NEEDMOREPARAMS(client->getNickname(), "PASS"));
		return ;
	}

	std::string password = arguments[0];
	if (arguments[0][0] == ':')
	{
		password = arguments[0].substr(1);
	}

	if (server->getPassword() != password)
	{
		client->sendMessageToClientItself(ERR_PASSWDMISMATCH(client->getNickname()));
		return ;
	}

	client->setRegistrationLevel(LOGIN_PHASE);
}
