#include "Server.hpp"

void Server::recvNextLine(int eventFD)
{
	int byt;
	char buf[BUFFER_SIZE];
	std::list<std::string> msgList;
	User *messenger = findUserByFD(eventFD);

	memset(buf, 0, BUFFER_SIZE);
	byt = recv(eventFD, buf, BUFFER_SIZE, 0);
	if (byt == -1)
		throw std::runtime_error("Error while reading buffer from client.");
	if (byt == 0)
	{
		disconnectUser(eventFD);
		return ;
	}
	
	messenger->addToBuffer(buf);
	msgList = strSplit(messenger->getPersonalBuffer(), '\n');
	if (msgList.size() > 0)
	{
		for (auto &it : msgList)
		{
			std::cout << "\033[1;31m" << "processing:\n" << it << "\033[0m\n" << std::endl;
			findReply(it, eventFD);
		}
		if (findUserByFD(eventFD) != nullptr)
			messenger->clearBuffer();
	}
}

void Server::findReply(std::string fullMsg, int eventFD)
{
	std::vector<std::string> splitArgs; 
	std::stringstream ss(fullMsg);
	std::string word;
	while (ss >> word)
		splitArgs.push_back(word);
	size_t vecSize = splitArgs.size();
	User *messenger = findUserByFD(eventFD);
	if (messenger == nullptr)
		throw std::runtime_error("User missing after read");
	if (vecSize < 1)
		return ;
	else if (!splitArgs[0].compare("CAP"))
		send(eventFD, "421 CAP :No Cap\r\n", 17, 0);
	else if ((vecSize > 1) && (!splitArgs[0].compare("PASS")))
		messenger->setPassword(splitArgs[1]);
	else if (!splitArgs[0].compare("NICK"))
		rplNick(splitArgs, messenger);
	else if (!splitArgs[0].compare("USER"))
		rplUser(splitArgs, messenger);
	else if ((vecSize > 1) && (!splitArgs[0].compare("netcatter")))
		rplNetCatter(splitArgs[1], messenger);
	else if (messenger->isIncompleteUser())
		send(eventFD, "User incomplete\r\n", 17, 0);
	else if (!splitArgs[0].compare("TOPIC"))
		rplTopic(splitArgs, messenger);
	else if (!splitArgs[0].compare("PING"))
		sendReply(eventFD, RPL_PING(messenger->getSource(), messenger->getHostName()) + "\r\n");
	else if (!splitArgs[0].compare("PART"))
		rplPart(splitArgs, messenger);
	else if (!splitArgs[0].compare("QUIT"))
		rplQuit(splitArgs, messenger);
	else if (vecSize < 2)
		return ;
	else if (!splitArgs[0].compare("JOIN"))
		rplJoin(splitArgs, messenger);
	else if (!splitArgs[0].compare("PRIVMSG"))
		rplPrivmsg(splitArgs, messenger);
	else if (!splitArgs[0].compare("NOTICE"))
		rplNotice(splitArgs, messenger);
	else if (!splitArgs[0].compare("INVITE"))
		rplInvite(splitArgs, messenger);
	else if (!splitArgs[0].compare("KICK"))
		rplKick(splitArgs, messenger);
	else if (!splitArgs[0].compare("MODE"))
		rplMode(splitArgs, messenger);			
}

void Server::sendReply(int targetFD, std::string msg)
{
	std::cout << "\033[1;36m" << "just sent:\n" << msg << "to fd " << targetFD << "\033[0m\n" << std::endl; // FOR TESTING
	send(targetFD, msg.c_str(), msg.length(), 0);
}