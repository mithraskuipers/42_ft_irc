#include "Server.hpp"

void Server::checkFailure(int socket, const std::string &msg)
{
	if (socket < 0)
	{
		throw std::runtime_error(msg.c_str());
		close(socket);
	}
}

std::list<std::string> Server::strSplit(std::string str, char separator)
{
	std::list<std::string> strVec;
	std::string s;
	size_t i = 0;
    while (i < str.size()) 
	{
        if (str[i] == separator)
		{
            strVec.push_back(s);
            s.clear();
		}
		else
    		s += str[i];
        i++;
	}
	return (strVec);
}

bool Server::confirmOperator(std::string channelName, User *messenger)
{
	if (!findChannel(channelName)->isOperator(messenger->getUserFD()))
	{
		sendReply(messenger->getUserFD(), ERR_CHANOPRIVSNEEDED(messenger->getSource(), channelName) + "\r\n");
		return (0);
	}
	return (1);
}

Channel *Server::findChannel(std::string channelName)
{
	for (auto &i : _allChannels)
	{
		if (!i->getChannelName().compare(channelName))
			return (&(*i));
	}
	return (nullptr);
}

User *Server::findUserByNick(std::string nickName)
{
	for (auto const& i : _allUsers)
	{
		if (!i->getNickName().compare(nickName))
			return (&(*i));
	}
	return (nullptr);
}

User *Server::findUserByFD(int fd)
{
	for (auto const& i : _allUsers)
	{
		if (i->getUserFD() == fd)
			return (&(*i));
	}
	return (nullptr);
}

std::string Server::strJoinWithSpaces(std::vector<std::string> splitArgs, size_t startPoint)
{
	std::string msg;
	while (startPoint < splitArgs.size())
	{
		msg += splitArgs[startPoint];
		if (startPoint != splitArgs.size())
			msg += " ";
		startPoint++;
	}
	return (msg);
}

void Server::serverStdout(const std::string &firstMessageCombined)
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
	std::string str(buffer);
	std::cout << "[" << str << "] " << firstMessageCombined << std::endl;
}