#include "Replies.hpp"
#include "Server.hpp"

void Server::findCommand(std::string commandWithOptions, int eventFD)
{
	std::string command = commandWithOptions.substr(0, commandWithOptions.find(" "));
	std::string options = commandWithOptions.substr(commandWithOptions.find(" ") + 1);
	std::cout << "command = " << command << std::endl;
	std::cout << "options = " << options << std::endl;
	if (!command.compare("NICK"))
	{
		for (auto const& i : _allUsers)
		{
			if (i->getUserFD() == eventFD)
			{
				i->setNickName(options);
				// i->printUserPrivates();
				// std::string stringlelinerg = RPL_WELCOME(i->getNickName()) + "\r\n";
				// send(eventFD, stringlelinerg.c_str(), 48, 0);
				// std::cout << "sent " << stringlelinerg << std::endl;
				send(eventFD, "001 johnnyBravo!bde-meij@127.0.0.1 :Welcome johnnyBravo!bde-meij@127.0.0.1\r\n", 200, 0);
			}
		}
	}
	if (!command.compare("USER"))
	{
		for (auto const& i : _allUsers)
		{
			if (i->getUserFD() == eventFD)
			{
				i->setRealName(options.substr(options.find(':') + 1));
				std::vector<std::string> veccie;
				char separator = ' ';
				int j = 0;
				
				std::string s; 
				while (options[j] != '\0') 
				{
					if (options[j] != separator)
						s += options[j];
					else 
					{
						veccie.push_back(s);
						s.clear();
					}
					j++;
				}
				i->setUserName(veccie[1]);
				i->setHostName(veccie[2]);
				i->printUserPrivates();
			}
		}
	}

	// if (!command.compare("PING"))
	// 	for (auto const& i : _allUsers)
	// 	{
	// 		if (i->getUserFD() == eventFD)
	// 		{
	// 			i->setNickName(options);
	// 			// i->printUserPrivates();
	// 			// std::string stringlelinerg = RPL_WELCOME(i->getNickName()) + "\r\n";
	// 			// send(eventFD, stringlelinerg.c_str(), 48, 0);
	// 			// std::cout << "sent " << stringlelinerg << std::endl;
	// 			send(eventFD, ":johnnyBravo!bde-meij@127.0.0.1 PONG :127.0.0.1\r\n", 48, 0);
	// 		}
	// 	}
	// if (!command.compare("MODE"))
	// 	for (auto const& i : _allUsers)
	// 	{
	// 		if (i->getUserFD() == eventFD)
	// 		{
	// 			i->setNickName(options);
	// 			// i->printUserPrivates();
	// 			// std::string stringlelinerg = RPL_WELCOME(i->getNickName()) + "\r\n";
	// 			// send(eventFD, stringlelinerg.c_str(), 48, 0);
	// 			// std::cout << "sent " << stringlelinerg << std::endl;
	// 			send(eventFD, ":johnnyBravo!bde-meij@127.0.0.1 PONG :127.0.0.1\r\n", 48, 0);
	// 		}
	// 	}
}
