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
	std::cout << "\033[1;36m" << "just sent:\n" << msg << "to fd " << eventFD << "\033[0m\n" << std::endl; // FOR TESTING, RMV LATER
	send(eventFD, msg.c_str(), msg.length(), 0);
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

void Server::rplNick(std::vector<std::string> splitArgs, int eventFD)
{
	for (auto const& j : _allUsers)
	{
		if (!j->getNickName().compare(splitArgs[1]))
		{
			std::cout << "nickname already exists" << std::endl;
			return ;
		}
	}
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			i->setNickName(splitArgs[1]);
		}
	}
}

void Server::rplJoin(std::vector<std::string> splitArgs, int eventFD)
{
	bool tmp = 0;
	for (auto &j : _allChannels)
	{
		if (!j->getChannelName().compare(splitArgs[1]))
			tmp = j->getIsInviteOnly();
	}
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == eventFD)
		{
			if ((tmp == 1) && (i->isInvited(splitArgs[1]) == 0))
			{
				std::cout << "not invited" << std::endl;
				return ;
			}
			else
			{
				sendReply(eventFD, RPL_NAMREPLY(i->getSource(), splitArgs[1], i->getNickName()) + "\r\n");
				sendReply(eventFD, RPL_ENDOFNAMES(i->getSource(), splitArgs[1]) + "\r\n");
				sendReply(eventFD, RPL_JOIN(i->getSource(), splitArgs[1]) + "\r\n");
				i->addJoinedChannel(splitArgs[1]);
			}
		}
	}
	for (auto &j : _allChannels)
	{
		if (!j->getChannelName().compare(splitArgs[1]))
			return ;
	}
	_allChannels.push_back(new Channel(splitArgs[1]));
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

void Server::rplPrivmsg(std::vector<std::string> splitArgs, int eventFD)
{
	// /connect 127.0.0.1 6667 pw johnnyBravo
	// check if msgtarget exists

	// see if args[1] channel or user
	// if channel send to all other fds in channel
	// args[2] can be multiple strings, add them together

	size_t i = 2;
	std::string msg;
	while (i < splitArgs.size())
	{
		msg += splitArgs[i];
		if (i != splitArgs.size())
			msg += " ";
		i++;
	}

	std::string sauce = "";
	for (auto &q : _allChannels)
	{
		if (!q->getChannelName().compare(splitArgs[1]))
		{
			for (auto & d : _allUsers)
			{
				if (d->getUserFD() == eventFD)
					sauce = d->getSource();
			}
			for (auto g : _allUsers)
			{
				if ((g->getUserFD() != eventFD) && (g->isInChannel(splitArgs[1])))
					sendReply(g->getUserFD(), RPL_PRIVMSG(sauce, splitArgs[1], msg) + "\r\n");
			}
			return ;
		}
	}

	for (auto const& a : _allUsers)
	{
		if (!a->getNickName().compare(splitArgs[1]))
		{
			for (auto const& b : _allUsers)
			{
				if (b->getUserFD() == eventFD)
				{
					sendReply(a->getUserFD(), RPL_PRIVMSG(b->getSource(), splitArgs[1], msg) + "\r\n");
					return ;
				}
			}
		}
	}

	std::cout << "msg target not found" << std::endl;
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

void Server::rplInvite(std::vector<std::string> splitArgs, int eventFD)
{
	// check if invitee exists
	for (auto const& a : _allUsers)
	{
		if (!a->getNickName().compare(splitArgs[1]))
		{
			for (auto const& b : _allUsers)
			{
				if (b->getUserFD() == eventFD)
				{
					sendReply(a->getUserFD(), RPL_INVITE(b->getSource(), splitArgs[1], splitArgs[2]) + "\r\n");
					a->addInvitation(splitArgs[2]);
					return ;
				}
			}
		}
	}
	std::cout << "invited user not found" << std::endl;
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

void Server::rplTopic(std::vector<std::string> splitArgs, int eventFD)
{
	std::string sauce;
	if (splitArgs[2].empty())
		splitArgs.push_back("");

	size_t i = 2;
	std::string topic;
	while (i < splitArgs.size())
	{
		topic += splitArgs[i];
		if (i != splitArgs.size())
			topic += " ";
		i++;
	}
	for (auto const& j : _allUsers)
	{
		if (j->getUserFD() == eventFD)
			sauce = j->getSource();
	}
	for (auto const& k : _allUsers)
	{
		if (k->isInChannel(splitArgs[1]) == 1)
		{
			sendReply(k->getUserFD(), RPL_TOPIC(sauce, splitArgs[1], topic) + "\r\n");
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