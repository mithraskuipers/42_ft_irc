#include "Replies.hpp"
#include "Server.hpp"
#include <sstream>
#include <algorithm>

	// "001 johnnyBravo!bde-meij@127.0.0.1 :Welcome johnnyBravo!bde-meij@127.0.0.1\r\n"
	// /connect 127.0.0.1 6667 pw johnnyBravo

void Server::findCommand(std::string buffer, int eventFD)
{
	std::vector<std::string> splitArgs; 
	std::stringstream ss(buffer);
	std::string word;
	// if (!ss)
	while (ss >> word)
		splitArgs.push_back(word);
	if (!splitArgs.empty())
	{
		if (!splitArgs[0].compare("NICK"))
			cmdNick(splitArgs, eventFD);
		if (!splitArgs[0].compare("USER"))
			cmdUser(splitArgs, eventFD);
		if (!splitArgs[0].compare("PING"))
			cmdPing(eventFD);
		if (!splitArgs[0].compare("QUIT"))
			disconnectUser(eventFD);
		// if (!splitArgs[0].compare("MODE"))
		// 	cmdMode(eventFD);			
		// if (!splitArgs[0].compare("CAP"))
		// 	send(eventFD, "421  CAP :Unknown command", 50, 0);
	}
}

void Server::cmdNick(std::vector<std::string> splitArgs, int eventFD)
{
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			i->setNickName(splitArgs[1]);
			std::string msg = RPL_WELCOME(splitArgs[1]) + "\r\n";
			send(eventFD, msg.c_str(), msg.length(), 0);
		}
	}
}

void Server::cmdUser(std::vector<std::string> splitArgs, int eventFD)
{
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			i->setUserName(splitArgs[2]);
			i->setHostName(splitArgs[3]);
			i->setRealName(splitArgs);
			// std::string msg = RPL_WELCOME(i->getSource()) + "\r\n";
			// send(eventFD, msg.c_str(), msg.length(), 0);
		}
	}
}

void Server::cmdPing(int eventFD)
{
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			std::string msg = RPL_PING(i->getSource(), i->getHostName()) + "\r\n";
			send(eventFD, msg.c_str(), msg.size(), 0);
		}
	}
}

// void Server::cmdMode(int eventFD)
// {
// 	for (auto const& i : _allUsers)
// 	{
// 		if (i->getUserFD() == eventFD)
// 		{
// 			std::string msg = RPL_MODE(i->getSource(), "", "", "") + "\r\n";
// 			send(eventFD, msg.c_str(), msg.length(), 0);
// 		}
// 	}
// }