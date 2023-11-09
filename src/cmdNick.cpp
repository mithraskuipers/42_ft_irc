/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cmdNick.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/07 22:01:02 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/09 22:29:31 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Command.hpp"
#include "./../incs/cmdNick.hpp"
#include "./../incs/Replies.hpp"

cmdNick::cmdNick(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{
}

cmdNick::~cmdNick()
{
}

void cmdNick::run(Client *client, std::vector<std::string> arguments)
{

	if (arguments.empty() || arguments[0].empty())
	{
		client->sendMessageToClientItself(ERR_NONICKNAMEGIVEN(client->getNickname()));
		return ;
	}

	std::string nickname = arguments[0];

	if (server->getClientInstance(nickname))
	{
		client->sendMessageToClientItself(ERR_NICKNAMEINUSE(client->getNickname()));
		return ;
	}
	client->setNickname(nickname);
	client->welcomeMessage();
}