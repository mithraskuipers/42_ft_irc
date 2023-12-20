#include "Replies.hpp"
#include "Server.hpp"
#include <sstream>
#include <stdlib.h>
#include <algorithm>

void Server::computeReply(std::string buffer, int eventFD)
{
	std::vector<std::string> splitArgs; 
	std::stringstream ss(buffer);
	std::string word;
	while (ss >> word)
		splitArgs.push_back(word);
	if (!splitArgs.empty())
	{
		User *messenger = findUserByFD(eventFD);
		if (!splitArgs[0].compare("PASS"))
			rplPass(splitArgs, messenger);
		if (!splitArgs[0].compare("NICK"))
			rplNick(splitArgs, messenger);
		if (!splitArgs[0].compare("USER"))
			rplUser(splitArgs, messenger);

		if (!splitArgs[0].compare("JOIN"))
			rplJoin(splitArgs, messenger);
		if (!splitArgs[0].compare("PART"))
			rplPart(splitArgs, messenger);
		if (!splitArgs[0].compare("PRIVMSG"))
			rplPrivmsg(splitArgs, messenger);
		if (!splitArgs[0].compare("QUIT"))
			rplQuit(splitArgs, messenger);

		if (!splitArgs[0].compare("INVITE"))
			rplInvite(splitArgs, messenger);
		if (!splitArgs[0].compare("KICK"))
			rplKick(splitArgs, messenger);
		if (!splitArgs[0].compare("MODE"))
			rplMode(splitArgs, messenger);			
		if (!splitArgs[0].compare("TOPIC"))
			rplTopic(splitArgs, messenger);

		if (!splitArgs[0].compare("CAP"))
			send(eventFD, "421 CAP :Unknown command\r\n", 26, 0);
		if (!splitArgs[0].compare("PING"))
			sendReply(eventFD, RPL_PING(messenger->getSource(), messenger->getHostName()) + "\r\n");
	}
}

void Server::sendReply(int targetFD, std::string msg)
{
	// implement EPOLLOUT here. Makes no noticeable difference but satisfies mandatory, maybe
	_currentlyHandledEvent.events = EPOLLOUT;

	std::cout << "\033[1;36m" << "just sent:\n" << msg << "to fd " << targetFD << "\033[0m\n" << std::endl; // FOR TESTING, RMV LATER
	send(targetFD, msg.c_str(), msg.length(), 0);

	// reset to epollin after send
	_currentlyHandledEvent.events = EPOLLIN;
}

void Server::rplPass(std::vector<std::string> splitArgs, User *messenger)
{
	messenger->setPassword(splitArgs[1]);
}

void Server::rplNick(std::vector<std::string> splitArgs, User *messenger)
{
	if (!messenger->getNickName().empty())
	{
		if (findUserByNick(splitArgs[1]) != nullptr)
		{
			sendReply(messenger->getUserFD(), ERR_NICKNAMEINUSE(messenger->getSource(), splitArgs[1]) + "\r\n");
			return ;
		}
		sendReply(messenger->getUserFD(), RPL_NICK(messenger->getSource(), splitArgs[1]) + "\r\n");
	}
	messenger->setNickName(splitArgs[1]);
}

void Server::rplUser(std::vector<std::string> splitArgs, User *messenger)
{
	messenger->setUserName(splitArgs[2]);
	messenger->setHostName(splitArgs[3]);
	messenger->setRealName(strJoinWithSpaces(splitArgs, 4));

	if (messenger->getPassword().compare(_password))
	{
		sendReply(messenger->getUserFD(), ERR_PASSWDMISMATCH(messenger->getSource()) + "\r\n");
		disconnectUser(messenger->getUserFD());
	}
	else if (findUserByNick(messenger->getNickName())->getUserFD() != messenger->getUserFD())
	{
		sendReply(messenger->getUserFD(), RPL_TRYAGAIN(messenger->getSource(), messenger->getNickName()) + "\r\n");
		disconnectUser(messenger->getUserFD());
	}
	else
	{
		sendReply(messenger->getUserFD(), RPL_WELCOME(messenger->getNickName(), messenger->getNickName()) + "\r\n"); 
		sendReply(messenger->getUserFD(), RPL_YOURHOST(messenger->getSource(), "ft_irc_serv", "3.42") + "\r\n");
		sendReply(messenger->getUserFD(), RPL_CREATED(messenger->getSource(), "today") + "\r\n");
		sendReply(messenger->getUserFD(), RPL_MYINFO(messenger->getSource(), "ft_irc_serv", "3.42", "o(perator)", "i,t,k,l") + "\r\n");
	}
}

void Server::rplJoin(std::vector<std::string> splitArgs, User *messenger)
{
	Channel *channel = findChannel(splitArgs[1]);

	if (channel == nullptr)
	{
		_allChannels.push_back(new Channel(splitArgs[1]));
		channel = findChannel(splitArgs[1]);
		channel->addToOperators(messenger->getUserFD());
	}
	if (channel->isBanned(messenger->getUserFD()))
	{
		sendReply(messenger->getUserFD(), ERR_BANNEDFROMCHAN(messenger->getSource(), channel->getChannelName()) + "\r\n");
		return ;
	}
	if ((channel->getChannelKey().find('k') != channel->getActiveModes().npos) && \
		(channel->getChannelKey().compare(splitArgs[2])))
	{
		sendReply(messenger->getUserFD(), ERR_BADCHANNELKEY(messenger->getSource(), \
		channel->getChannelName()) + "\r\n");
		return ;
	}
	if ((channel->getActiveModes().find('i') != channel->getActiveModes().npos) && \
		(!messenger->isInvited(splitArgs[1])))
	{
		sendReply(messenger->getUserFD(), ERR_INVITEONLYCHAN(messenger->getSource(), \
		channel->getChannelName()) + "\r\n");
		return ;
	}
	if ((channel->getActiveModes().find('l') != channel->getActiveModes().npos) && \
		(channel->getNumOfUsers() >= channel->getLimit()))
	{
		sendReply(messenger->getUserFD(), ERR_CHANNELISFULL(messenger->getSource(), \
		channel->getChannelName()) + "\r\n");
		return ;
	}
	channel->addToChannel(messenger->getUserFD());
	channel->msgAllInChannel(RPL_JOIN(messenger->getSource(), splitArgs[1]) + "\r\n");
	sendReply(messenger->getUserFD(), RPL_TOPIC(messenger->getSource(), channel->getChannelName(), channel->getTopic()) + "\r\n");
	for (auto &j: _allUsers)
	{
		if ((channel->isInChannel(j->getUserFD())))
			sendReply(messenger->getUserFD(), RPL_NAMREPLY(messenger->getSource(), splitArgs[1], j->getNickName()) + "\r\n");
	}
	sendReply(messenger->getUserFD(), RPL_ENDOFNAMES(messenger->getSource(), splitArgs[1]) + "\r\n");
}

void Server::rplPrivmsg(std::vector<std::string> splitArgs, User *messenger)
{
	Channel *channel = findChannel(splitArgs[1]);
	std::string msg = strJoinWithSpaces(splitArgs, 2);
	if (channel != nullptr)
	{
		for (auto const &i : _allUsers)
		{
			if (i->getUserFD() != messenger->getUserFD() && channel->isInChannel(i->getUserFD()))
				sendReply(i->getUserFD(), RPL_PRIVMSG(messenger->getSource(), splitArgs[1], msg) + "\r\n");
		}
	}
	else if (findUserByNick(splitArgs[1]) != nullptr) // SEGFAULTS WHEN SEARCHING PRE-EXISTING USER
		sendReply(findUserByNick(splitArgs[1])->getUserFD(), RPL_PRIVMSG(messenger->getSource(), splitArgs[1], msg) + "\r\n");
	else
		std::cout << "msgreceiver does not exist" << std::endl;
}

void Server::rplQuit(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs[1].empty())
		splitArgs.push_back("");
	sendReply(messenger->getUserFD(), RPL_QUIT(messenger->getSource(), splitArgs[1]) + "\r\n");
	disconnectUser(messenger->getUserFD());
}

void Server::rplInvite(std::vector<std::string> splitArgs, User *messenger)
{	
	User *invitee = findUserByNick(splitArgs[1]);
	if (invitee == nullptr)
	{
		std::cout << "invited user not found" << std::endl;
		return ;
	}
	if (confirmOperator(splitArgs[2], messenger))
	{
		sendReply(invitee->getUserFD(), RPL_INVITE(messenger->getSource(), \
		splitArgs[1], splitArgs[2]) + "\r\n");
		invitee->addInvitation(splitArgs[2]);
	}
}

void Server::rplKick(std::vector<std::string> splitArgs, User *messenger)
{
	User *creep;
	if (splitArgs.size() < 4)
		splitArgs.push_back("");
	creep = findUserByNick(splitArgs[2]);
	(void) messenger;
	if (creep == nullptr)
		std::cout << "kick target not found" << std::endl;
	else if (confirmOperator(splitArgs[1], messenger))
	{
		rplPart(splitArgs, creep);
		findChannel(splitArgs[1])->addToBanned(creep->getUserFD());
		findChannel(splitArgs[1])->msgAllInChannel(RPL_KICK(messenger->getSource(), \
		splitArgs[1], splitArgs[2], strJoinWithSpaces(splitArgs, 3)) + "\r\n");
	}
}

std::string cleanModes(std::string unclean)
{
	std::string clean;
	size_t i = 0;
	clean += unclean[0];
	while (i < unclean.length())
	{
		if ((unclean[i] == 'i') || (unclean[i] == 't') || (unclean[i] == 'k') || (unclean[i] == 'l'))
			clean += unclean[i];
		i++;
	}
	return (clean);
}

void Server::setRmvLimit(std::vector<std::string> splitArgs, Channel *channel)
{
	if (splitArgs[2][0] == '-')
	{
		channel->setLimit(-1);
		return ;
	}

	size_t i = 3;
	while (i < splitArgs.size())
	{
		// if ((splitArgs[i].all_of(splitArgs.begin(), splitArgs.end(), std::string::isdigit)) && 
		if (findUserByNick(splitArgs[i]) == nullptr)
		{
			channel->setLimit(std::stoi(splitArgs[i]));
		}
		i++;
	}
}

void Server::addRmvOperator(std::vector<std::string> splitArgs, Channel *channel)
{
	size_t i = 3;
	while (i < splitArgs.size())
	{
		if (channel->isInChannel(findUserByNick(splitArgs[i])->getUserFD()))
		{
			if (splitArgs[2][0] == '+')
				channel->addToOperators(findUserByNick(splitArgs[i])->getUserFD());
			if (splitArgs[2][0] == '-')
				channel->rmvFromOperators(findUserByNick(splitArgs[i])->getUserFD());
		}
		i++;
	}
}

void Server::setRmvChannelKey(std::vector<std::string> splitArgs, Channel *channel)
{
	// size_t i = 3;
	// while (i < splitArgs.size())
	// {
	// 	if (channel->isInChannel(findUserByNick(splitArgs[i])->getUserFD()))
	// 	{
	if (splitArgs[2][0] == '+')
		channel->setChannelKey(splitArgs[3]);
	if (splitArgs[2][0] == '-')
		channel->setChannelKey("");
		// }
		// i++;
	// }
}

void Server::rplMode(std::vector<std::string> splitArgs, User *messenger)
{
	Channel *channel = (findChannel(splitArgs[1]));
	if (channel != nullptr)
	{
		if (splitArgs.size() == 2)
		{
			std::cout << "active modes requested, currently: " << channel->getActiveModes() << std::endl;
			sendReply(messenger->getUserFD(), RPL_MODE(messenger->getSource(), \
			splitArgs[1], channel->getActiveModes() + "\r\n"));
		}
		else if (confirmOperator(splitArgs[1], messenger))
		{
			channel->setActiveModes(splitArgs[2]);

			if (splitArgs[2].find('o') != std::string::npos)
				addRmvOperator(splitArgs, channel);
			if (splitArgs[2].find('l') != std::string::npos)
				setRmvLimit(splitArgs, channel);
			if (splitArgs[2].find('k') != std::string::npos)
				setRmvChannelKey(splitArgs, channel);

			channel->msgAllInChannel(RPL_MODE(messenger->getSource(), \
			splitArgs[1], cleanModes(splitArgs[2]) + "\r\n"));
		}
	}
}

void Server::rplTopic(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs[2].empty())
		splitArgs.push_back("");
	Channel *channel = findChannel(splitArgs[1]);

	if  (channel->getActiveModes().find('t') != std::string::npos)
	{
		if (confirmOperator(splitArgs[1], messenger))
			channel->setTopic(strJoinWithSpaces(splitArgs, 2));
		else
			return ;
	}
	else
		channel->setTopic(strJoinWithSpaces(splitArgs, 2));

	for (auto const& u : _allUsers)
	{
		if (channel->isInChannel(u->getUserFD()))
		{
			sendReply(u->getUserFD(), RPL_TOPIC(messenger->getSource(), \
			channel->getChannelName(), channel->getTopic()) + "\r\n");
		}
	}
}

void Server::rplPart(std::vector<std::string> splitArgs, User *messenger)
{
	Channel *channel = findChannel(splitArgs[1]);
	channel->msgAllInChannel(RPL_PART(messenger->getSource(), splitArgs[1]) + "\r\n");
	channel->rmvFromChannel(messenger->getUserFD());
	channel->rmvFromOperators(messenger->getUserFD());
	if (channel->getNumOfOperators() < 1)
		channel->addToOperators(channel->getFirstJoinedUserFD());
}
