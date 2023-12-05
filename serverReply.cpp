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
		if (!splitArgs[0].compare("NICK"))
			rplNick(splitArgs, eventFD);
		if (!splitArgs[0].compare("USER"))
			rplUser(splitArgs, eventFD);
		if (!splitArgs[0].compare("PING"))
			rplPing(eventFD);
		if (!splitArgs[0].compare("QUIT"))
			disconnectUser(eventFD);
		if (!splitArgs[0].compare("WHOIS"))
			rplWhois(eventFD);
		// if (!splitArgs[0].compare("MODE"))
		// 	rplMode(eventFD);			
		// if (!splitArgs[0].compare("CAP"))
		// 	send(eventFD, "421  CAP :Unknown command", 50, 0);
	}
}

void Server::sendReply(int eventFD, std::string msg)
{
	std::cout << "\033[1;36m" << "just sent:\n" << msg << "\033[0m\n" << std::endl; // FOR TESTING, RMV LATER
	send(eventFD, msg.c_str(), msg.length(), 0);
}

void Server::rplWhois(int eventFD)
{
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			sendReply(eventFD, RPL_WHOISUSER(i->getNickName(), i->getUserName(), i->getHostName(), i->getRealName()) + "\r\n");
		}
	}
}

void Server::rplNick(std::vector<std::string> splitArgs, int eventFD)
{
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			i->setNickName(splitArgs[1]);
		}
	}
}

void Server::rplUser(std::vector<std::string> splitArgs, int eventFD)
{
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			i->setUserName(splitArgs[2]);
			i->setHostName(splitArgs[3]);
			i->setRealName(splitArgs);
			sendReply(eventFD, RPL_WELCOME(i->getSource()) + "\r\n");
		}
	}
}

void Server::rplPing(int eventFD)
{
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			sendReply(eventFD, RPL_PING(i->getSource(), i->getHostName()) + "\r\n");
		}
	}
}

// void Server::rplMode(int eventFD)
// {
// 	for (auto const& i : _allUsers)
// 	{
// 		if (i->getUserFD() == eventFD)
// 		{
// 			sendReply(eventFD, RPL_MODE(i->getSource(), "", "", "") + "\r\n");
//
// 		}
// 	}
// }