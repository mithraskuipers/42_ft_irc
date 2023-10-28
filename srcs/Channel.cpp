/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/21 20:49:24 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/28 14:53:54 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Channel.hpp"
#include "./../incs/Client.hpp"
#include "./../incs/includes.hpp"

/*
********************************************************************************
Orthodox canonical form
********************************************************************************
*/

Channel::Channel() : topicAuthor(""), topic("")
{
}

Channel::~Channel()
{
}

Channel::Channel(const std::string &name) : channelName(name), topicAuthor(""), topic("")
{
}

/*
********************************************************************************
Getters
********************************************************************************
*/

std::string Channel::getName() const
{
	return (channelName);
}

std::string Channel::getTopic() const
{
	return (topic);
}

std::string Channel::getTopicAuthor() const
{
	return (topicAuthor);
}

int Channel::getClientsCount() const
{
	return (_clients.size());
}

/*
********************************************************************************
Setters
********************************************************************************
*/

void Channel::setTopic(const std::string &newTopic, Client *client)
{
	topic = newTopic;
	topicAuthor = client->getNick();
}

/*
********************************************************************************
Member functions
********************************************************************************
*/

void Channel::addClient(Client *client)
{
	_clients.push_back(client);
}

void Channel::removeClient(Client *client)
{
    auto it = std::find(_clients.begin(), _clients.end(), client);
    if (it != _clients.end())
    {
        _clients.erase(it);
    }
}


bool Channel::isClientInChannel(const Client *client) const
{
	if (std::find(_clients.begin(), _clients.end(), client) != _clients.end())
		return (TRUE);
	return (FALSE);
}

void Channel::broadcastMessage(const std::string &message, Client *sender)
{
	for (auto &client : _clients)
	{
		if (client != sender)
		{
			client->sendToClient(message);
		}
	}
}

bool Channel::isEmpty() const
{
	return (_clients.empty());
}

bool Channel::isOperator(Client *client) const
{
	auto it = std::find(operators.begin(), operators.end(), client);
	return (it != operators.end());
}

void Channel::addOperator(Client *client)
{
	operators.push_back(client);
}

void Channel::removeOperator(Client *client)
{
	auto it = std::find(operators.begin(), operators.end(), client);
	if (it != operators.end())
	{
		operators.erase(it);
	}
}

void Channel::listClients() const
{
	std::cout << "Clients currently in the channel " << channelName << ":\n";
	for (const auto &client : _clients)
	{
		std::cout << "- " << client->getNick() << "\n";
	}
}
