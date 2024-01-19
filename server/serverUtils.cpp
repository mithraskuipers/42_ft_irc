#include "Server.hpp"

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

Channel *Server::findChannel(std::string channelName)
{
	for (auto &i : _allChannels)
	{
		if (!i->getChannelName().compare(channelName))
			return (i);
	}
	return (nullptr);
}

User *Server::findUserByNick(std::string nickName)
{
	for (auto &i : _allUsers)
	{
		if (!i->getNickName().compare(nickName))
			return (i);
	}
	return (nullptr);
}

User *Server::findUserByFD(int fd)
{
	for (auto &i : _allUsers)
	{
		if (i->getUserFD() == fd)
			return (i);
	}
	return (nullptr);
}


