// Command.cpp

#include "./../incs/Command.hpp"
#include "./../incs/User.hpp"
#include "./../incs/Channel.hpp"
#include <algorithm>
#include <unistd.h>
#include "./../incs/IRCServer.hpp"

class IRCServer; // Forward declaration of IRCServer class

Command::Command(std::vector<User> &clients, IRCServer &server) : clients(clients), ircServer(server)
{
	// Constructor implementation
}

Command::~Command()
{
	// Destructor implementation
}

void Command::addChannel(const std::string &channelName)
{
	Channel newChannel(channelName);
	channels.push_back(newChannel);
}

void Command::handlePassCommand(const std::vector<std::string> &command, User &client)
{
	if (command.size() >= 2)
	{
		std::string password = command[1];
		password = password.substr(0, ircServer.getPass().length());

		if (password == ircServer.getPass())
		{
			client.setRegistered(true);
			std::string successMessage = ":server 001 " + client.getNick() + " :You have successfully logged in, " + client.getNick() + "!\r\n";
			client.sendToClient(successMessage);
			std::cout << "User " << client.getNick() << " has successfully logged in." << std::endl;
		}
		else
		{
			client.sendToClient(":server 464 " + client.getNick() + " :Password incorrect\r\n");
			std::cout << "User " << client.getNick() << " failed to log in. Incorrect password." << std::endl;
		}
	}
	else
	{
		client.sendToClient(":server 461 " + client.getNick() + " PASS :Not enough parameters\r\n");
	}
}

void Command::handleNickCommand(const std::vector<std::string> &command, User &client)
{
	if (command.size() >= 2)
	{
		std::string newNick = command[1];

		if (!newNick.empty())
		{
			bool nicknameInUse = std::any_of(clients.begin(), clients.end(), [&newNick](const User &user)
											 { return user.getNick() == newNick; });

			if (!nicknameInUse)
			{
				std::string oldNick = client.getNick();
				client.setNick(newNick);

				for (auto &channel : channels)
				{
					if (channel.isUserInChannel(&client))
					{
						std::string nickChangeMessage = ":" + oldNick + " NICK " + newNick + "\r\n";
						channel.broadcastMessage(nickChangeMessage, &client);
					}
				}

				client.sendToClient("Your nickname has been changed to: " + newNick + "\n");
				std::cout << "User " << oldNick << " has changed their nickname to " << newNick << std::endl;

				std::string nickChangeResponse = ":server 001 " + newNick + " :Your new nickname is " + newNick + "\r\n";
				client.sendToClient(nickChangeResponse);
			}
			else
			{
				client.sendToClient(":server 433 * " + newNick + " :Nickname is already in use\r\n");
			}
		}
		else
		{
			client.sendToClient(":server 431 * :No nickname given\r\n");
		}
	}
	else
	{
		client.sendToClient(":server 431 * :No nickname given\r\n");
	}
}

void Command::handleUserCommand(const std::vector<std::string> &command, User &client)
{
	if (command.size() >= 5)
	{
		std::string username = command[1];
		std::string realname = command[4];

		client.setNick(username);
		client.setRealName(realname);

		client.sendToClient(":server 001 " + client.getNick() + " :Welcome to the IRC server, " + client.getNick() + "!\r\n");

		std::cout << "User " << client.getNick() << " registered with username: " << username << " and real name: " << realname << std::endl;
	}
	else
	{
		client.sendToClient(":server 461 " + client.getNick() + " USER :Not enough parameters\r\n");
	}
}

void Command::handleQuitCommand(const std::vector<std::string> &command, User &client)
{
	std::string quitMessage = command.size() >= 2 ? command[1] : "Client Quit";

	auto iterator = std::find_if(clients.begin(), clients.end(), [&client](const User &user)
								 { return user.getSocketDescriptor() == client.getSocketDescriptor(); });

	if (iterator != clients.end())
	{
		clients.erase(iterator);
	}

	close(client.getSocketDescriptor());

	auto it = std::find_if(clients.begin(), clients.end(), [&client](const User &user)
						   { return user.getSocketDescriptor() == client.getSocketDescriptor(); });

	if (it != clients.end())
	{
		clients.erase(it);
	}

	std::cout << "User " << client.getNick() << " has quit the server. Reason: " << quitMessage << std::endl;
}

void Command::handleJoinCommand(const std::vector<std::string> &command, User &client)
{
	if (command.size() >= 2)
	{
		std::string channelName = command[1];
		bool alreadyInChannel = std::any_of(channels.begin(), channels.end(), [&client, &channelName](const Channel &channel)
											{ return channel.getName() == channelName && channel.isUserInChannel(&client); });

		if (!alreadyInChannel)
		{
			auto it = std::find_if(channels.begin(), channels.end(), [&channelName](const Channel &channel)
								   { return channel.getName() == channelName; });

			if (it != channels.end())
			{
				it->addUser(&client);
				client.sendToClient("Joined channel: " + channelName + "\n");
				std::string joinMessage = ":" + client.getNick() + " JOIN " + channelName + "\r\n";
				client.sendToClient(joinMessage);
			}
			else
			{
				client.sendToClient("Channel not found: " + channelName + "\n");
			}
		}
		else
		{
			client.sendToClient("You are already in the channel " + channelName + ".\n");
		}
	}
	else
	{
		client.sendToClient(":server 461 " + client.getNick() + " JOIN :Not enough parameters\r\n");
	}
}

void Command::handleLeaveCommand(const std::vector<std::string> &command, User &client)
{
	if (command.size() >= 2)
	{
		std::string channelName = command[1];
		auto it = std::find_if(channels.begin(), channels.end(), [&channelName](const Channel &channel)
							   { return channel.getName() == channelName; });

		if (it != channels.end())
		{
			if (it->isUserInChannel(&client))
			{
				it->removeUser(&client);
				client.sendToClient("You left channel: " + channelName + "\n");
			}
			else
			{
				client.sendToClient("You are not in channel: " + channelName + ".\n");
			}
		}
		else
		{
			client.sendToClient("Channel not found: " + channelName + ".\n");
		}
	}
	else
	{
		client.sendToClient(":server 461 " + client.getNick() + " LEAVE :Not enough parameters\r\n");
	}
}

void Command::handleListCommand(const std::vector<std::string> &command, User &client)
{
	(void)command;
	(void)client;
}

void Command::handleCreateCommand(const std::vector<std::string> &command, User &client)
{
	if (command.size() >= 2)
	{
		std::string channelName = command[1];
		bool channelExists = std::any_of(channels.begin(), channels.end(), [&channelName](const Channel &channel)
										 { return channel.getName() == channelName; });

		if (!channelExists)
		{
			Channel newChannel(channelName);
			newChannel.addUser(&client);
			channels.push_back(newChannel);

			std::string joinMessage = ":" + client.getNick() + " JOIN :" + channelName + "\r\n";
			std::string topicMessage = ":" + client.getNick() + " TOPIC " + channelName + " :Welcome to " + channelName + " channel!\r\n";
			std::string endOfNamesMessage = ":server 366 " + client.getNick() + " " + channelName + " :End of NAMES list\r\n";

			client.sendToClient("Channel created: " + channelName + "\n");

			for (auto &user : channels.back().getUsers())
			{
				user->sendToClient(joinMessage);
				user->sendToClient(topicMessage);
				user->sendToClient(endOfNamesMessage);
			}
		}
		else
		{
			client.sendToClient(":server 403 " + client.getNick() + " " + channelName + " :Channel already exists\r\n");
		}
	}
	else
	{
		client.sendToClient(":server 461 " + client.getNick() + " CREATE :Not enough parameters\r\n");
	}
}

void Command::sendChannelList(User &user)
{
	if (channels.empty())
	{
		user.sendToClient("There are no channels in this server.\n");
	}
	else
	{
		std::string channelList = "Channel List:\n";
		for (const auto &channel : channels)
		{
			channelList += channel.getName() + " (" + std::to_string(channel.getUsersCount()) + " users)\n";
		}
		user.sendToClient(channelList);
	}
}

void Command::commandHandler(const std::string &input, User &client)
{
	// Split the input into command and arguments
	std::vector<std::string> command;
	size_t spacePos = input.find(' ');
	command.push_back(input.substr(0, spacePos)); // Extract the command

	if (spacePos != std::string::npos)
	{
		command.push_back(input.substr(spacePos + 1)); // Extract the arguments
	}

	if (command[0] == "pass")
	{
		handlePassCommand(command, client);
	}
	else if (command[0] == "nick")
	{
		handleNickCommand(command, client);
	}
	else if (command[0] == "user")
	{
		handleUserCommand(command, client);
	}
	else if (command[0] == "quit")
	{
		handleQuitCommand(command, client);
	}
	else if (command[0] == "join")
	{
		handleJoinCommand(command, client);
	}
	else if (command[0] == "leave")
	{
		handleLeaveCommand(command, client);
	}
	else if (command[0] == "list")
	{
		handleListCommand(command, client);
	}
	else if (command[0] == "create")
	{
		handleCreateCommand(command, client);
	}
	else
	{
		// Handle unknown command logic or send an error message
		client.sendToClient(":server ERROR :Unknown command\r\n");
	}
}
