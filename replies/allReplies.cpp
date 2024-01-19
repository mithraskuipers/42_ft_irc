#include "../server/Server.hpp"

void Server::rplJoin(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs.size() < 2)
	{
		sendReply(messenger->getUserFD(), ERR_NEEDMOREPARAMS(messenger->getSource(), "JOIN") + "\r\n");
		return ;
	}
	Channel *channel = findChannel(splitArgs[1]);
	if (channel == nullptr)
	{
		_allChannels.push_back(new Channel(splitArgs[1]));
		channel = findChannel(splitArgs[1]);
		channel->addToOperators(messenger->getUserFD());
		std::cout << "operator added " << std::endl;
	}
	if (checkJoinErrors(channel, messenger, splitArgs))
		return ;
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

void Server::rplInvite(std::vector<std::string> splitArgs, User *messenger)
{	
	if (splitArgs.size() < 3)
	{
		sendReply(messenger->getUserFD(), ERR_NEEDMOREPARAMS(messenger->getSource(), "INVITE") + "\r\n");
		return ;
	}
	User *invitee = findUserByNick(splitArgs[1]);
	Channel *channel = findChannel(splitArgs[2]);
	if (invitee == nullptr)
	{
		sendReply(messenger->getUserFD(), ERR_NOSUCHNICK(messenger->getSource(), splitArgs[1]) + "\r\n");
		return ;
	}
	if (channel == nullptr)
	{
		sendReply(messenger->getUserFD(), ERR_NOSUCHCHANNEL(messenger->getSource(), splitArgs[2]) + "\r\n");
		return ;
	}
	if (confirmOperator(splitArgs[2], messenger))
	{
		sendReply(invitee->getUserFD(), RPL_INVITE(messenger->getSource(), \
		splitArgs[1], splitArgs[2]) + "\r\n");
		invitee->addInvitation(splitArgs[2]);
		channel->rmvFromBanned(invitee->getUserFD());
	}
}

void Server::rplTopic(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs.size() < 2)
	{
		sendReply(messenger->getUserFD(), ERR_NEEDMOREPARAMS(messenger->getSource(), "TOPIC") + "\r\n");
		return ;
	}
	if (splitArgs.size() < 3)
		splitArgs.push_back("");
	Channel *channel = findChannel(splitArgs[1]);
	if (channel == nullptr)
	{
		sendReply(messenger->getUserFD(), ERR_NOSUCHCHANNEL(messenger->getSource(), splitArgs[2]) + "\r\n");
		return ;
	}
	if  (channel->getActiveModes().find('t') != std::string::npos)
	{
		if (confirmOperator(splitArgs[1], messenger))
			channel->setTopic(strJoinWithSpaces(splitArgs, 2));
		else
			return ;
	}
	else
		channel->setTopic(strJoinWithSpaces(splitArgs, 2));

	channel->msgAllInChannel(RPL_TOPIC(messenger->getSource(), \
	channel->getChannelName(), channel->getTopic()) + "\r\n");
}

void Server::rplNick(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs.size() < 2)
	{
		sendReply(messenger->getUserFD(), ERR_NEEDMOREPARAMS(messenger->getSource(), "NICK") + "\r\n");
		return ;
	}
	std::string nickname = splitArgs[1];
	if (checkNick(messenger, nickname))
		return ;
	if (!messenger->getNickName().empty())
		sendReply(messenger->getUserFD(), RPL_NICK(messenger->getSource(), nickname) + "\r\n");
	messenger->setNickName(nickname);
}

void Server::rplNetCatter(std::string password, User *messenger)
{
	messenger->setUserName("netcatter");
	messenger->setHostName(HOSTNAME);
	messenger->setRealName("netcatter");
	messenger->setPassword(password);
	checkPassWord(messenger);
}

void Server::rplUser(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs.size() < 4)
	{
		sendReply(messenger->getUserFD(), ERR_NEEDMOREPARAMS(messenger->getSource(), "USER") + "\r\n");
		return ;
	}
	if (messenger->getNickName().empty())
	{
		disconnectUser(messenger->getUserFD());
		return ;
	}
	messenger->setUserName(splitArgs[2]);
	messenger->setHostName(splitArgs[3]);
	messenger->setRealName(strJoinWithSpaces(splitArgs, 4));
	checkPassWord(messenger);
}

void Server::rplMode(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs.size() < 2)
	{
		sendReply(messenger->getUserFD(), ERR_NEEDMOREPARAMS(messenger->getSource(), "MODE") + "\r\n");
		return ;
	}
	if (findUserByNick(splitArgs[1]) != nullptr) // protection from 'other' MODE
		return ;
	Channel *channel = (findChannel(splitArgs[1]));
	if (channel == nullptr)
	{
		sendReply(messenger->getUserFD(), ERR_NOSUCHCHANNEL(messenger->getSource(), splitArgs[1]) + "\r\n");
		return ;
	}
	if (splitArgs.size() == 2)
	{
		if (channel->getActiveModes().size() < 2)
			channel->setActiveModes("");
		sendReply(messenger->getUserFD(), RPL_MODE(messenger->getSource(), \
		splitArgs[1], channel->getActiveModes() + "\r\n"));
	}
	else if (confirmOperator(splitArgs[1], messenger))
	{
		channel->setActiveModes(splitArgs[2]);
		if (splitArgs[2][0] == '+')
			modeArgsPlus(splitArgs, channel);
		if (splitArgs[2][0] == '-')
			modeArgsMinus(splitArgs, channel);
		channel->msgAllInChannel(RPL_MODE(messenger->getSource(), \
		splitArgs[1], cleanModes(splitArgs[2]) + "\r\n"));
	}
}

void Server::rplPrivmsg(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs.size() < 2)
	{
		sendReply(messenger->getUserFD(), ERR_NEEDMOREPARAMS(messenger->getSource(), "MSG") + "\r\n");
		return ;
	}
	Channel *channel = findChannel(splitArgs[1]);
	std::string msg = strJoinWithSpaces(splitArgs, 2);
	if (channel != nullptr)
	{
		if (!channel->isInChannel(messenger->getUserFD()))
			sendReply(messenger->getUserFD(), ERR_NOTONCHANNEL(messenger->getSource(), channel->getChannelName()) + "\r\n");
		else for (auto const &i : _allUsers)
		{
			if (i->getUserFD() != messenger->getUserFD() && channel->isInChannel(i->getUserFD()))
				sendReply(i->getUserFD(), RPL_PRIVMSG(messenger->getSource(), splitArgs[1], msg) + "\r\n");
		}
	}
	else if (findUserByNick(splitArgs[1]) != nullptr)
		sendReply(findUserByNick(splitArgs[1])->getUserFD(), RPL_PRIVMSG(messenger->getSource(), splitArgs[1], msg) + "\r\n");
	else
		sendReply(messenger->getUserFD(), ERR_NOSUCHNICK(messenger->getSource(), splitArgs[1]) + "\r\n");
}

void Server::rplNotice(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs.size() < 2)
		return ;
	Channel *channel = findChannel(splitArgs[1]);
	std::string msg = strJoinWithSpaces(splitArgs, 2);
	if (channel != nullptr)
	{
		if (!channel->isInChannel(messenger->getUserFD()))
			return ;
		else for (auto const &i : _allUsers)
		{
			if (i->getUserFD() != messenger->getUserFD() && channel->isInChannel(i->getUserFD()))
				sendReply(i->getUserFD(), RPL_NOTICE(messenger->getSource(), splitArgs[1], msg) + "\r\n");
		}
	}
	else if (findUserByNick(splitArgs[1]) != nullptr)
		sendReply(findUserByNick(splitArgs[1])->getUserFD(), RPL_NOTICE(messenger->getSource(), splitArgs[1], msg) + "\r\n");
}

void Server::rplQuit(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs.size() < 2)
		splitArgs.push_back("");
	sendReply(messenger->getUserFD(), RPL_QUIT(messenger->getSource(), splitArgs[1]) + "\r\n");
	leaveAllChannels(messenger);
	disconnectUser(messenger->getUserFD());
}

void Server::rplKick(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs.size() < 3)
	{
		sendReply(messenger->getUserFD(), ERR_NEEDMOREPARAMS(messenger->getSource(), "KICK") + "\r\n");
		return ;
	}
	User *creep = findUserByNick(splitArgs[2]);
	Channel *channel = findChannel(splitArgs[1]);
	if (splitArgs.size() < 4)
		splitArgs.push_back("");
	if (creep == nullptr)
	{
		sendReply(messenger->getUserFD(), ERR_NOSUCHNICK(messenger->getSource(), splitArgs[1]) + "\r\n");
		return ;
	}
	if (channel == nullptr)
	{
		sendReply(messenger->getUserFD(), ERR_NOSUCHCHANNEL(messenger->getSource(), splitArgs[2]) + "\r\n");
		return ;
	}
	else if (confirmOperator(splitArgs[1], messenger))
	{
		userLeavesChannel(splitArgs[1], creep);
		channel = findChannel(splitArgs[1]);
		if (channel == nullptr)
			return ;
		channel->addToBanned(creep->getUserFD());
		channel->msgAllInChannel(RPL_KICK(messenger->getSource(), \
		splitArgs[1], splitArgs[2], strJoinWithSpaces(splitArgs, 3)) + "\r\n");
	}
}

void Server::rplPart(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs.size() < 2)
	{
		sendReply(messenger->getUserFD(), ERR_NEEDMOREPARAMS(messenger->getSource(), "PART") + "\r\n");
		return ;
	}
	Channel *channel = findChannel(splitArgs[1]);
	if (channel == nullptr)
	{
		sendReply(messenger->getUserFD(), ERR_NOSUCHCHANNEL(messenger->getSource(), splitArgs[1]) + "\r\n");
		return ;
	}
	userLeavesChannel(splitArgs[1], messenger);
}