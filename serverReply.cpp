#include "Replies.hpp"
#include "Server.hpp"
#include <sstream>
#include <algorithm>

	// "001 johnnyBravo!bde-meij@127.0.0.1 :Welcome johnnyBravo!bde-meij@127.0.0.1\r\n"
	// /connect 127.0.0.1 6667 pw johnnyBravo

	// join = send: :johnnyBravo!bde-meij@127.0.0.1 JOIN :#kamer
	// join = send: :johnnyBravo!bde-meij@127.0.0.1 JOIN #kamer
	// part = send: :johnnyBravo!bde-meij@127.0.0.1 PART :#kamer
	// part = send: :johnnyBravo!bde-meij@127.0.0.1 PART #kamer

void Server::computeReply(std::string buffer, int eventFD)
{
	std::vector<std::string> splitArgs; 
	std::stringstream ss(buffer);
	std::string word;
	while (ss >> word)
		splitArgs.push_back(word);
	if (!splitArgs.empty())
	{
		if (!splitArgs[0].compare("USER"))
			rplUser(splitArgs, eventFD);
		if (!splitArgs[0].compare("NICK"))
			rplNick(splitArgs, eventFD);

		if (!splitArgs[0].compare("JOIN"))
			rplJoin(splitArgs, eventFD);
		if (!splitArgs[0].compare("PART"))
			rplPart(splitArgs, eventFD);
		if (!splitArgs[0].compare("PRIVMSG")) // privmsg is implied when in channel talking to all others
			rplPrivmsg(splitArgs, eventFD);
		if (!splitArgs[0].compare("QUIT"))
			rplQuit(splitArgs, eventFD);

		if (!splitArgs[0].compare("INVITE"))
			rplInvite(splitArgs, eventFD);
		if (!splitArgs[0].compare("KICK"))
			rplKick(splitArgs, eventFD);

		// if (!splitArgs[0].compare("MODE"))
		// 	rplMode(eventFD);			
		if (!splitArgs[0].compare("TOPIC"))
			rplTopic(splitArgs, eventFD);

		if (!splitArgs[0].compare("CAP")) // if we implement CAP LS, negotiation must be finished before registering user
			send(eventFD, "421 CAP :Unknown command\r\n", 26, 0);
		if (!splitArgs[0].compare("PING"))
			rplPing(eventFD);
		if (!splitArgs[0].compare("WHOIS"))
			rplWhois(eventFD);
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

void Server::rplTopic(std::vector<std::string> splitArgs, int eventFD)
{
	if (splitArgs[2].empty())
		splitArgs.push_back("");
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			sendReply(eventFD, RPL_TOPIC(i->getSource(), splitArgs[1], splitArgs[2]) + "\r\n");
		}
	}
}

void Server::rplKick(std::vector<std::string> splitArgs, int eventFD)
{
	int partFD;
	if (splitArgs[3].empty())
		splitArgs.push_back("");
	for (auto const& a : _allUsers)
	{
		if (a->getNickName() == splitArgs[2])
		{
			partFD = a->getUserFD();
			for (auto const& b : _allUsers)
			{
				if (b->getUserFD() == eventFD)
				{
					sendReply(eventFD, RPL_KICK(b->getSource(), splitArgs[1], splitArgs[2], splitArgs[3]) + "\r\n");
					sendReply(partFD, RPL_PART(a->getSource(), splitArgs[1]) + "\r\n");
					return ;
				}
			}
		}
	}
	std::cout << "user who should be kicked was not found" << std::endl;
}

void Server::rplQuit(std::vector<std::string> splitArgs, int eventFD)
{
	if (splitArgs[1].empty())
		splitArgs.push_back("");
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			sendReply(eventFD, RPL_QUIT(i->getSource(), splitArgs[1]) + "\r\n");
			disconnectUser(eventFD);
		}
	}
}

void Server::rplJoin(std::vector<std::string> splitArgs, int eventFD)
{
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			sendReply(eventFD, RPL_NAMREPLY(i->getSource(), splitArgs[1], i->getNickName()) + "\r\n");
			sendReply(eventFD, RPL_ENDOFNAMES(i->getSource(), splitArgs[1]) + "\r\n");
			sendReply(eventFD, RPL_JOIN(i->getSource(), splitArgs[1]) + "\r\n");
		}
	}
}

void Server::rplPart(std::vector<std::string> splitArgs, int eventFD)
{
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			sendReply(eventFD, RPL_PART(i->getSource(), splitArgs[1]) + "\r\n");
		}
	}
}

void Server::rplInvite(std::vector<std::string> splitArgs, int eventFD)
{
	// check if invitee exists
	for (auto const& a : _allUsers)
	{
		if (a->getNickName() == splitArgs[1])
		{
			for (auto const& b : _allUsers)
			{
				if (b->getUserFD() == eventFD)
				{
					sendReply(eventFD, RPL_INVITE(b->getSource(), splitArgs[1], splitArgs[2]) + "\r\n");
					return ;
				}
			}
		}
	}
	std::cout << "invited user not found" << std::endl;
}

void Server::rplPrivmsg(std::vector<std::string> splitArgs, int eventFD)
{
	// check if msgtarget exists
	for (auto const& a : _allUsers)
	{
		if (a->getNickName() == splitArgs[1])
		{
			for (auto const& b : _allUsers)
			{
				if (b->getUserFD() == eventFD)
				{
					sendReply(eventFD, RPL_PRIVMSG(b->getSource(), splitArgs[1], splitArgs[2]) + "\r\n");
					return ;
				}
			}
		}
	}
	std::cout << "msg target not found" << std::endl;
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

			// 001    RPL_WELCOME
			// "Welcome to the Internet Relay Network
			// <nick>!<user>@<host>"

			// 002    RPL_YOURHOST
			// "Your host is <servername>, running version <ver>"

			// 003    RPL_CREATED
			// "This server was created <date>"

			// 004    RPL_MYINFO
			// "<servername> <version> <available user modes>
			// <available channel modes>"
			
			// - The server sends Replies 001 to 004 to a user upon
			// successful registration.


			// PART doesnt work when RPL_WELCOME(1'source' 2'nickname') though protocol states use source on welcomeMSG
			// might be that names should be stored / sent differently altogether
			sendReply(eventFD, RPL_WELCOME(i->getNickName(), i->getNickName()) + "\r\n"); 

			// sendReply(eventFD, RPL_YOURHOST(i->getSource(), "ft_irc_serv", "3.42") + "\r\n");
			// sendReply(eventFD, RPL_CREATED(i->getSource(), "today") + "\r\n");
			// sendReply(eventFD, RPL_MYINFO(i->getSource(), "ft_irc_serv", "3.42", "o(perator)", "i,t,k,l") + "\r\n");
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

// 		}
// 	}
// }