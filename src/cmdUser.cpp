/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cmdUser.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 14:09:04 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/06 14:09:05 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Command.hpp"
#include "./../incs/cmdUser.hpp"
#include "./../incs/Replies.hpp"

/*
********************************************************************************
ORTHODOX CANONICAL FORM
********************************************************************************
*/

cmdUser::cmdUser(Server *server, bool registrationRequired) : Command(server, registrationRequired)
{

}

cmdUser::~cmdUser()
{

}

/*
********************************************************************************
MEMBER FUNCTIONS
********************************************************************************
*/

void cmdUser::execute(Client *client, std::vector<std::string> arguments)
{

	if (client->isRegistered())
	{
		client->sendMessageToClientItself(ERR_ALREADYREGISTERED(client->getNickname()));
		return ;
	}

	if (arguments.size() < 4)
	{
		client->sendMessageToClientItself(ERR_NEEDMOREPARAMS(client->getNickname(), "USER"));
		return ;
	}
	client->setUsername(arguments[0]);
	client->setRealName(arguments[3]);
	client->welcomeMessage();
}