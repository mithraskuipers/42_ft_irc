#include "Server.hpp"

int Server::checkJoinErrors(Channel *channel, User *messenger, std::vector<std::string> splitArgs)
{
	if (channel->isBanned(messenger->getUserFD()))
		sendReply(messenger->getUserFD(), ERR_BANNEDFROMCHAN(messenger->getSource(), channel->getChannelName()) + "\r\n");
	else if ((channel->getActiveModes().find('k') != channel->getActiveModes().npos) && \
		((splitArgs.size() < 3) || (channel->getChannelKey().compare(splitArgs[2]))))
		sendReply(messenger->getUserFD(), ERR_BADCHANNELKEY(messenger->getSource(), channel->getChannelName()) + "\r\n");
	else if ((channel->getActiveModes().find('i') != channel->getActiveModes().npos) && (!messenger->isInvited(channel->getChannelName())))
		sendReply(messenger->getUserFD(), ERR_INVITEONLYCHAN(messenger->getSource(), channel->getChannelName()) + "\r\n");
	else if ((channel->getActiveModes().find('l') != channel->getActiveModes().npos) && (channel->getNumOfUsers() >= channel->getLimit()))
		sendReply(messenger->getUserFD(), ERR_CHANNELISFULL(messenger->getSource(), channel->getChannelName()) + "\r\n");
	else
		return (0);
	return (1);
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