#include "../server/Server.hpp"

bool Server::confirmOperator(std::string channelName, User *messenger)
{
	if (!findChannel(channelName)->isOperator(messenger->getUserFD()))
	{
		sendReply(messenger->getUserFD(), ERR_CHANOPRIVSNEEDED(messenger->getSource(), channelName) + "\r\n");
		return (0);
	}
	return (1);
}

int Server::checkJoinErrors(Channel *channel, User *messenger, std::vector<std::string> splitArgs)
{
	int ufd = messenger->getUserFD();
	std::string source = messenger->getSource();
	std::string channelName = channel->getChannelName();

	if (channel->isInChannel(ufd))
		sendReply(ufd, "you are already in the channel\r\n");
	else if (channel->isBanned(ufd))
		sendReply(ufd, ERR_BANNEDFROMCHAN(source, channelName) + "\r\n");
	else if ((channel->getActiveModes().find('k') != channel->getActiveModes().npos) && \
		((splitArgs.size() < 3) || (channel->getChannelKey().compare(splitArgs[2]))))
		sendReply(ufd, ERR_BADCHANNELKEY(source, channelName) + "\r\n");
	else if ((channel->getActiveModes().find('i') != channel->getActiveModes().npos) && \
		(!messenger->isInvited(channelName)))
		sendReply(ufd, ERR_INVITEONLYCHAN(source, channelName) + "\r\n");
	else if ((channel->getActiveModes().find('l') != channel->getActiveModes().npos) && \
		(channel->getNumOfUsers() >= channel->getLimit()))
		sendReply(ufd, ERR_CHANNELISFULL(source, channelName) + "\r\n");
	else
		return (0);
	return (1);
}

bool Server::checkNick(User *messenger, std::string nickname)
{
	size_t i = 0;
	while (i < nickname.size())
	{
		if (!std::isalnum(nickname[i]))
		{
			sendReply(messenger->getUserFD(), \
			ERR_ERRONEUSNICKNAME(messenger->getSource(), nickname) + "\r\n");
			return (1);
		}
		i++;
	}
	if (findUserByNick(nickname) != nullptr)
	{
		sendReply(messenger->getUserFD(), \
		ERR_NICKNAMEINUSE(messenger->getSource(), nickname) + "\r\n");
		return (1);
	}
	return (0);
}

void Server::checkPassWord(User *messenger)
{
	if (messenger->getPassword().compare(_password))
	{
		sendReply(messenger->getUserFD(), ERR_PASSWDMISMATCH(messenger->getSource()) + "\r\n");
		disconnectUser(messenger->getUserFD());
	}
	else
	{
		sendReply(messenger->getUserFD(), RPL_WELCOME(messenger->getNickName(), messenger->getNickName()) + "\r\n"); 
		sendReply(messenger->getUserFD(), RPL_YOURHOST(messenger->getSource(), "ircServer", "3.42") + "\r\n");
		sendReply(messenger->getUserFD(), RPL_CREATED(messenger->getSource(), "today") + "\r\n");
		sendReply(messenger->getUserFD(), RPL_MYINFO(messenger->getSource(), "ircServer", "3.42", "o(perator)", "i,t,k,l") + "\r\n");
	}
}

void Server::modeArgsPlus(std::vector<std::string> splitArgs, Channel *channel)
{
	size_t i = 0;
	size_t n_arg = 3;
	while (i < splitArgs[2].size() && n_arg < splitArgs.size())
	{
		if (splitArgs[2][i] == 'o')
			channel->addToOperators(findUserByNick(splitArgs[n_arg])->getUserFD());
		else if (splitArgs[2][i] == 'l')
			channel->setLimit(stoi(splitArgs[n_arg]));
		else if (splitArgs[2][i] == 'k')
			channel->setChannelKey(splitArgs[n_arg]);
		else
			n_arg--;
		n_arg++;
		i++;
	}
}

void Server::modeArgsMinus(std::vector<std::string> splitArgs, Channel *channel)
{
	size_t i = 0;
	size_t n_arg = 3;
	while (i < splitArgs[2].size() && n_arg < splitArgs.size())
	{
		if (splitArgs[2][i] == 'o')
		{
			channel->rmvFromOperators(findUserByNick(splitArgs[n_arg])->getUserFD());
			n_arg++;
		}
		if (splitArgs[2][i] == 'l')
			channel->setLimit(-1);
		if (splitArgs[2][i] == 'k')
			channel->setChannelKey("");
		i++;
	}
}

std::string Server::cleanModes(std::string unclean)
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

void Server::userLeavesChannel(std::string channelName, User *messenger)
{
	Channel *channel = findChannel(channelName);
	channel->msgAllInChannel(RPL_PART(messenger->getSource(), channel->getChannelName()) + "\r\n");
	channel->rmvFromOperators(messenger->getUserFD());
	channel->rmvFromChannel(messenger->getUserFD());
	if (channel->getNumOfUsers() < 1)
	{
		_allChannels.remove(findChannel(channel->getChannelName()));
		delete(channel);
	}
	else if (channel->getNumOfOperators() < 1)
		channel->addToOperators(channel->getFirstJoinedUserFD());
}

void Server::leaveAllChannels(User *messenger)
{
	bool search = 0;
	while (search == 0)
	{
		search = 1;
		for (auto &i : _allChannels)
		{
			if (i->isInChannel(messenger->getUserFD()))
			{
				userLeavesChannel(i->getChannelName(), messenger);
				search = 0;
				break ;
			}
		}
	}
}