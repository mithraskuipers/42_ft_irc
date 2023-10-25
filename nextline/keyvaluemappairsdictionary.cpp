#include <unordered_map>
#include <iostream>
#include <fstream>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include "get_next_line.hpp"
#include <vector>

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef  void (*funk)(int);

void kick(int fd)
{
	std::cout << "kick was called on fd " << fd << std::endl;
}

void invite(int fd)
{
	std::cout << "invite was called on fd " << fd << std::endl;
}

void topic(int fd)
{
	std::cout << "topic was called on fd " << fd << std::endl;
}

void mode(int fd)
{
	std::cout << "mode was called on fd " << fd << std::endl;
}

int commander(int fd, std::string command)
{
	// std::cout << "command is " << command << "in command" << std::endl;
	std::unordered_map<std::string, funk> moep;

	moep["KICK\n"] = kick;
	moep["INVITE\n"] = invite;
	moep["TOPIC\n"] = topic;
	moep["MODE\n"] = mode;

	// std::map<std::string, int>::iterator it = moep.begin();
	for (auto &it : moep)
	{
		std::cout << "this is command = " << command << std::endl;
		if (!it.first.compare(command))
		{
			std::cout << "this is command2 = " << command << std::endl;
			std::cout << "past IF" << std::endl;
			it.second(fd);
			return (1);
		}
	}
	return (0);
		// std::cout << it.first << " " << it.second << std::endl;
}

// vector if you need no pointers to the elements
// 

void initUsers(struct pollfd *allegedUsers)
{
	allegedUsers[0].fd = 0;
	allegedUsers[0].events = POLLIN;
	allegedUsers[1].fd = 0;
	allegedUsers[1].events = POLLIN;
	allegedUsers[2].fd = 0;
	allegedUsers[2].events = POLLIN;
	allegedUsers[3].fd = 0;
	allegedUsers[3].events = POLLIN;
	allegedUsers[4].fd = 0;
	allegedUsers[4].events = POLLIN;
}

int main()
{
	struct pollfd allegedUsers[5];
	initUsers(allegedUsers);
	std::vector<struct pollfd> polledUsers;
	std::string message;
	for (auto &i : allegedUsers)
		polledUsers.push_back(i);
	int i = 0;
	while (i != -1)
	{
		if (i == 5)
			i = 0;
		poll(&polledUsers[i], 1, 100);
		if (polledUsers[i].revents &POLLIN)
		{
			message.append(get_next_line(polledUsers[i].fd));
			if (!commander(polledUsers[i].fd, message))
				std::cout << "user says: " << message << std::endl;
			message.clear();
		}
		i++;
	}
}



