#include "Replies.hpp"
#include "Server.hpp"
#include <sstream>
#include <algorithm>

	// "001 johnnyBravo!bde-meij@127.0.0.1 :Welcome johnnyBravo!bde-meij@127.0.0.1\r\n"
	// /connect 127.0.0.1 6667 pw johnnyBravo

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
		if (!splitArgs[0].compare("USER"))
			rplUser(splitArgs, messenger);
		if (!splitArgs[0].compare("NICK"))
			rplNick(splitArgs, messenger);

		if (!splitArgs[0].compare("JOIN"))
			rplJoin(splitArgs, messenger);
		if (!splitArgs[0].compare("PART"))
			rplPart(splitArgs, messenger);
		if (!splitArgs[0].compare("PRIVMSG")) // privmsg is implied when in channel talking to all others
			rplPrivmsg(splitArgs, messenger);
		if (!splitArgs[0].compare("QUIT"))
			rplQuit(splitArgs, messenger);

		if (!splitArgs[0].compare("INVITE"))
			rplInvite(splitArgs, messenger);
		if (!splitArgs[0].compare("KICK"))
			rplKick(splitArgs, messenger);

		// if (!splitArgs[0].compare("MODE"))
		// 	rplMode(messenger);			
		if (!splitArgs[0].compare("TOPIC"))
			rplTopic(splitArgs, messenger);

		if (!splitArgs[0].compare("CAP")) // if we implement CAP LS, negotiation must be finished before registering user
			send(eventFD, "421 CAP :Unknown command\r\n", 26, 0);
		if (!splitArgs[0].compare("PING"))
			sendReply(eventFD, RPL_PING(messenger->getSource(), messenger->getHostName()) + "\r\n");
		if (!splitArgs[0].compare("WHOIS"))
			rplWhois(messenger);
	}
}

void Server::sendReply(int targetFD, std::string msg)
{
	// implement EPOLLOUT here
	// _currentlyHandledEvent.events = EPOLLOUT;

	std::cout << "\033[1;36m" << "just sent:\n" << msg << "to fd " << targetFD << "\033[0m\n" << std::endl; // FOR TESTING, RMV LATER
	send(targetFD, msg.c_str(), msg.length(), 0);

	// reset to epollin after send
	// _currentlyHandledEvent.events = EPOLLIN;
}

void Server::rplUser(std::vector<std::string> splitArgs, User *messenger)
{
	messenger->setUserName(splitArgs[2]);
	messenger->setHostName(splitArgs[3]);
	messenger->setRealName(strJoinWithSpaces(splitArgs, 4));
	
	// PART doesnt work when RPL_WELCOME(1'source' 2'nickname') though protocol states use source on welcomeMSG
	// might be that names should be stored / sent differently altogether
	sendReply(messenger->getUserFD(), RPL_WELCOME(messenger->getNickName(), messenger->getNickName()) + "\r\n"); 

	// sendReply(messenger->getUserFD(), RPL_YOURHOST(messenger->getSource(), "ft_irc_serv", "3.42") + "\r\n");
	// sendReply(messenger->getUserFD(), RPL_CREATED(messenger->getSource(), "today") + "\r\n");
	// sendReply(messenger->getUserFD(), RPL_MYINFO(messenger->getSource(), "ft_irc_serv", "3.42", "o(perator)", "i,t,k,l") + "\r\n");
}

void Server::rplNick(std::vector<std::string> splitArgs, User *messenger)
{
	User *user = findUserByNick(splitArgs[1]);
	if (user != nullptr)
	{
		std::cout << "nickname already exists on other user" << std::endl;
		return ;
	}
	messenger->setNickName(splitArgs[1]);
}

void Server::rplJoin(std::vector<std::string> splitArgs, User *messenger)
{
	Channel *channel = findChannel(splitArgs[1]);
	bool inviteOnly = 0;

	if (channel != nullptr)
		inviteOnly = channel->getIsInviteOnly();
	else
		_allChannels.push_back(new Channel(splitArgs[1]));
	if (inviteOnly && !messenger->isInvited(splitArgs[1]))
	{
		std::cout << "not invited" << std::endl;
		return ;
	}
	sendReply(messenger->getUserFD(), RPL_NAMREPLY(messenger->getSource(), splitArgs[1], messenger->getNickName()) + "\r\n");
	sendReply(messenger->getUserFD(), RPL_ENDOFNAMES(messenger->getSource(), splitArgs[1]) + "\r\n");
	sendReply(messenger->getUserFD(), RPL_JOIN(messenger->getSource(), splitArgs[1]) + "\r\n");
	messenger->addJoinedChannel(splitArgs[1]);
}

void Server::rplPrivmsg(std::vector<std::string> splitArgs, User *messenger)
{
	std::string msg = strJoinWithSpaces(splitArgs, 2);
	if (findChannel(splitArgs[1]) != nullptr)
	{
		for (auto const &i : _allUsers)
		{
			if ((i->getUserFD() != messenger->getUserFD()) && (i->isInChannel(splitArgs[1])))
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
	// check if invitee exists
	User *invitee = findUserByNick(splitArgs[1]);
	if (invitee == nullptr)
	{
		std::cout << "invited user not found" << std::endl;
		return ;
	}
	sendReply(invitee->getUserFD(), RPL_INVITE(messenger->getSource(), \
	splitArgs[1], splitArgs[2]) + "\r\n");
	invitee->addInvitation(splitArgs[2]);
}

void Server::rplKick(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs[3].empty())
		splitArgs.push_back("");
	User *creep = findUserByNick(splitArgs[2]);
	if (creep == nullptr)
	{
		std::cout << "kick target not found" << std::endl;
	}
	sendReply(creep->getUserFD(), RPL_KICK(messenger->getSource(), \
	splitArgs[1], splitArgs[2], splitArgs[3]) + "\r\n");
	sendReply(creep->getUserFD(), RPL_PART(creep->getSource(), splitArgs[1]) + "\r\n");
}

// void Server::rplMode(User *messenger)
// {
// 	for 
// 		{
// 			sendReply(messenger->getUserFD(), RPL_MODE(i->getSource(), "", "", "") + "\r\n");

// 		}
// 	}
// }

void Server::rplTopic(std::vector<std::string> splitArgs, User *messenger)
{
	if (splitArgs[2].empty())
		splitArgs.push_back("");
	std::string topic = strJoinWithSpaces(splitArgs, 2);

	for (auto const& u : _allUsers)
	{
		if (u->isInChannel(splitArgs[1]))
		{
			sendReply(u->getUserFD(), RPL_TOPIC(messenger->getSource(), \
			splitArgs[1], topic) + "\r\n");
		}
	}
}

void Server::rplWhois(User *messenger)
{
	sendReply(messenger->getUserFD(), RPL_WHOISUSER(messenger->getNickName(), messenger->getUserName(), \
	messenger->getHostName(), messenger->getRealName()) + "\r\n");
}

// void Server::rplPing(User *messenger)
// {
//
// 	sendReply(messenger->getUserFD(), RPL_PING(messenger->getSource(), messenger->getHostName()) + "\r\n");
// }

void Server::rplPart(std::vector<std::string> splitArgs, User *messenger)
{
	sendReply(messenger->getUserFD(), RPL_PART(messenger->getSource(), splitArgs[1]) + "\r\n");
	messenger->rmvJoinedChannel(splitArgs[1]);
}