/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 14:59:44 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/09 22:36:37 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "./../incs/Client.hpp"
#include "./../incs/Server.hpp"
#include "./../incs/Replies.hpp"
#include "./../incs/utils.hpp"

/*
********************************************************************************
MEMBER FUNCTIONS
********************************************************************************
*/

/*
Maken van afzender prefix. Dit is eenmaal hoe IRC werkt.
Stel Bas stuurt een private firstMessageCombined commando naar Sparky
/privmsg Sparky Hoe gaat het?
Dan maakt irssi hiervan
/PRIVMSG Sparky :Hoe gaat het?
Dit is wat de server dus ontvangt. Note dat er een : is toegevoegd. Die moet worden verwijderd in latere stap.
Om het bericht van Bas door te sturen naar Sparky in een format die conform IRC is, moet er nog wat aan worden veranderd.
Er moet een zogeheten afzender "prefix" voor het bericht worden geplakt.
:<nickname>!<username>@<hostname> PRIVMSG <target> :<bericht>
:Bas!Bassie@127.0.0.1 PRIVMSG Sparky :Hoe gaat het?
Het is dit uiteindelijk bericht wat wordt gestuurd naar de ontvangende client via Client::sendMessageToClientItself()

Maar let op:
Als er geen <username> en <hostname> zijn, wordt de prefix: :<nickname>
Als er alleen een <username> is, wordt de prefix: :<nickname>!<username>
Als er alleen een <hostname> is (onwaarschijnlijk voor een gebruiker, maar mogelijk voor servers), wordt de prefix: ::@<hostname>
*/

// Onderstaand nog work in progress. Krijg het niet helemaal werkend met irssi. Vreemde tekens voor de namen in channels, zoals <@Bas> or < :Mithras> ipv <Bas> en <Mithras>

// std::string Client::getPrefix() const
// {
// 	std::string prefix = clientNickname;

// 	if (!clientUsername.empty())
// 	{
// 		prefix = ":" + prefix + "!" + clientUsername;
// 	}

// 	if (!_hostname.empty())
// 	{
// 		prefix = prefix + "@" + _hostname;
// 	}

// 	return (prefix);
// }

std::string Client::getPrefix() const
{
	std::string prefix = clientNickname;

	if (!clientUsername.empty())
	{
		prefix = prefix + "!" + clientUsername;
	}

	if (!_hostname.empty())
	{
		prefix = prefix + "@" + _hostname;
	}

	return (prefix);
}


bool Client::isInChannel(const std::string &channelName) const
{
	return (_channel && _channel->getChannelName() == channelName);
}

void Client::sendMessageToClientItself(const std::string &firstMessageCombined) const
{
	std::string buffer = firstMessageCombined + "\r\n";
	if (send(_fd, buffer.c_str(), buffer.length(), 0) < 0)
	{
		throw std::runtime_error("Error while sending firstMessageCombined to client.");
	}
}

void Client::welcomeMessage()
{
	if (clientRegistrationLevel != LOGIN_PHASE || clientUsername.empty() || clientRealname.empty() || clientNickname.empty())
	{
		return ;
	}
	clientRegistrationLevel = CHATTING_PHASE;

	sendMessageToClientItself(RPL_WELCOME(clientNickname));						// Important for IRC connection (?)

	std::ostringstream messageStream;
	messageStream << _hostname << ":" << port << " is now known as " << clientNickname << ".";
	std::string firstMessageCombined = messageStream.str();

	serverStdout(firstMessageCombined);
}


// WIP
void Client::clientInvite(std::string target, std::string channel)
{
	// if (1) // check if allowed for user (if op condition is set, check if operator)
		_server->singleBroadcast(RPL_INVITE(clientNickname, target, channel), target);
}

void Client::clientSetTopic(std::string channel, std::string topic)
{
	// if (1) // check if allowed for user (if op condition is set, check if operator)
		sendMessageToClientItself(RPL_TOPIC(clientNickname, channel, topic));
}
// END OF WIP

void Client::clientJoinChannel(Channel *channel)
{
	channel->addClient(this);
	_channel = channel;

	std::ostringstream usersStream;
	std::vector<std::string> nicknames = channel->getNicknames();
	for (const auto &nickname : nicknames)
	{
		usersStream << nickname << " ";
	}
	std::string users = usersStream.str();

	sendMessageToClientItself(RPL_NAMREPLY(clientNickname, channel->getChannelName(), users));
	sendMessageToClientItself(RPL_ENDOFNAMES(clientNickname, channel->getChannelName()));

	channel->channelBroadcast(RPL_JOIN(getPrefix(), channel->getChannelName()));

	std::ostringstream messageStream;
	messageStream << clientNickname << " has joined channel " << channel->getChannelName() << ".";
	std::string firstMessageCombined = messageStream.str();

	serverStdout(firstMessageCombined);
}

/*
Let client leave the current channel.
At the moment the client can only be in a single channel.
*/

void Client::clientLeaveChannel()
{
	if (!_channel)
	{
		return ;
	}
	const std::string name = _channel->getChannelName();

	sendMessageToClientItself(RPL_PART(getPrefix(), _channel->getChannelName()));
	_channel->removeClient(this);

	std::ostringstream messageStream;
	messageStream << clientNickname << " has left channel " << name << ".";
	std::string firstMessageCombined = messageStream.str();

	serverStdout(firstMessageCombined);
}

/*
********************************************************************************
ORTHODOX CANONICAL FORM
********************************************************************************
*/

Client::Client(int fd, const std::string &hostname, int port, Server *server) : _fd(fd), _hostname(hostname), port(port), _server(server), clientRegistrationLevel(INIT_CONNECTION_PHASE), _channel(nullptr)
{
}

Client::~Client()
{
}

/*
********************************************************************************
GETTERS / SETTERS
********************************************************************************
*/

int Client::getFD() const
{
	return (_fd);
};

std::string Client::getHostname() const
{
	return (_hostname);
};

int Client::getPort() const
{
	return (port);
};

bool Client::isRegistered() const
{
	return clientRegistrationLevel == CHATTING_PHASE;
};

std::string Client::getNickname() const
{
	return (clientNickname);
};

std::string Client::getUsername() const
{
	return (clientUsername);
};

std::string Client::getRealName() const
{
	return (clientRealname);
};

Channel *Client::getChannelInstance() const
{
	return (_channel);
};

void Client::setNickname(const std::string &nickname)
{
	clientNickname = nickname;
};

void Client::setUsername(const std::string &username)
{
	clientUsername = username;
};

void Client::setRealName(const std::string &realname)
{
	clientRealname = realname;
};

void Client::setRegistrationLevel(int updatedRegistrationLevel)
{
	clientRegistrationLevel = updatedRegistrationLevel;
};

void Client::setChannel(Channel *channel)
{
	_channel = channel;
};
