/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 14:59:46 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/09 22:17:17 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

/*
********************************************************************************
INCLUDES
********************************************************************************
*/

#include "./../incs/Channel.hpp"
#include "./../incs/Replies.hpp"
#include "./../incs/utils.hpp"

/*
********************************************************************************
MEMBER FUNCTIONS
********************************************************************************
*/

/*
Broadcast a firstMessageCombined to all clients in the channel.
If exclude parameter is provided, exclude the specified client from the broadcast.
*/

void Channel::channelBroadcast(const std::string &firstMessageCombined, Client* exclude /*= nullptr*/) // default value, making it optional
{
    for (clients_iterator it = _clients.begin(); it != _clients.end(); it++)
    {
        if (*it != exclude)
        {
            (*it)->sendMessageToClientItself(firstMessageCombined);
        }
    }
}

/*
Remove a client from the channel.
Check also if the client happened to be the channel admin.
If so, select new admin.
*/

void Channel::removeClient(Client *clientToBeRemoved)
{
	auto it = std::find(_clients.begin(), _clients.end(), clientToBeRemoved);
	if (it != _clients.end())
	{
		_clients.erase(it);
		clientToBeRemoved->setChannel(nullptr);

		if (channelAdminClient == clientToBeRemoved)
		{
			selectNewAdmin();
		}
	}
}

void Channel::selectNewAdmin()
{
	if (!_clients.empty())
	{
		channelAdminClient = _clients.front();
		logNewAdmin();
	}
	else
	{
		logEmptyChannel();
	}
}

void Channel::logNewAdmin()
{
	std::ostringstream logMessage;
	logMessage << channelAdminClient->getNickname() << " is now the admin of channel " << channelName << ".";
	serverStdout(logMessage.str());
}

void Channel::logEmptyChannel()
{
	serverStdout("Note: The channel " + channelName + " is empty.");
}

bool Channel::allowExternalMessagesToChannel() const
{
	return (channelAllowExternalMessages);
}

void Channel::addClient(Client *client)
{
	_clients.push_back(client);
};

/*
********************************************************************************
ORTHODOX CANONICAL FORM
********************************************************************************
*/

Channel::Channel(const std::string &name, const std::string &password, Client *admin) : channelName(name), channelAdminClient(admin), channelPassword(password), channelMaxClients(0), channelAllowExternalMessages(false)
{
}

Channel::~Channel()
{
}

/*
********************************************************************************
GETTERS / SETTERS
********************************************************************************
*/

/*
Return container with nickname strings in the current channel
*/

std::vector<std::string> Channel::getNicknames()
{
	std::string nickname;
	std::vector<std::string> nicknames;

	for (clients_iterator client_it = _clients.begin(); client_it != _clients.end(); client_it++)
	{
		Client *client = client_it.operator*();

		if (channelAdminClient == client)
		{
			nickname = "@" + (*client_it)->getNickname();
		}
		else
		{
			nickname = (*client_it)->getNickname();
		}

		nicknames.push_back(nickname);
	}
	return (nicknames);
}

Client *Channel::getAdmin()
{
	return (channelAdminClient);
}

std::string Channel::getChannelName() const
{
	return (channelName);
}

std::string Channel::getPassword() const
{
	return (channelPassword);
}

void Channel::setPassword(std::string password)
{
	this->channelPassword = password;
}

size_t Channel::getMaxClients() const
{
	return (channelMaxClients);
}

void Channel::setMaxClients(size_t nmaxclients)
{
	this->channelMaxClients = nmaxclients;
}

size_t Channel::getNumClients() const
{
	return (_clients.size());
}
