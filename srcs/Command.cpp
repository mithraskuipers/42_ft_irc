// Command.cpp

#include "./../incs/Command.hpp"
#include "./../incs/User.hpp"
#include "./../incs/Channel.hpp"
#include <algorithm>
#include <unistd.h>
#include "./../incs/IRCServer.hpp"

class IRCServer; // Forward declaration of IRCServer class

Command::Command(std::vector<User> &clients, IRCServer &server) : clients(clients), ircServer(server) {}

Command::~Command() {}

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

		// Remove newline characters from the newNick string
		newNick.erase(std::remove(newNick.begin(), newNick.end(), '\n'), newNick.end());

		if (!newNick.empty() && newNick != client.getNick()) // Check if newNick is not empty and different from the current nickname
		{
			bool nicknameInUse = std::any_of(clients.begin(), clients.end(), [&newNick](const User &user)
											 { return user.getNick() == newNick; });

			if (!nicknameInUse)
			{
				std::string oldNick = client.getNick(); // Get the current nickname before the change
				client.setNick(newNick);

				for (auto &channel : channels)
				{
					if (channel.isUserInChannel(&client))
					{
						std::string nickChangeMessage = ":" + oldNick + " NICK " + newNick + "\r\n";
						channel.broadcastMessage(nickChangeMessage, &client);
					}
				}

				// Inform the client about the nickname change
				std::string nickChangeResponse = ":" + oldNick + " NICK " + newNick + "\r\n";
				client.sendToClient(nickChangeResponse);

				std::cout << "User " << oldNick << " has changed their nickname to " << newNick << std::endl;
			}
			else
			{
				client.sendToClient(":server 433 * " + newNick + " :Nickname is already in use\r\n");
			}
		}
		else
		{
			client.sendToClient(":server 432 * :Erroneous Nickname\r\n");
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
	std::string quitMessage;
	if (command.size() >= 2)
	{
		quitMessage = command[1];
	}
	else
	{
		quitMessage = "Client quit";
	}

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

		// Ensure channel name starts with '#'
		if (channelName[0] != '#')
		{
			client.sendToClient(":server 403 " + client.getNick() + " " + channelName + " :Invalid channel name\r\n");
			return;
		}

		auto it = std::find_if(channels.begin(), channels.end(), [&channelName](const Channel &channel)
							   { return channel.getName() == channelName; });

		if (it == channels.end())
		{
			// Channel doesn't exist, create it
			Channel newChannel(channelName);
			newChannel.addUser(&client);
			channels.push_back(newChannel);

			// Notify the client about channel creation
			std::string joinMessage = ":" + client.getNick() + " JOIN " + channelName + "\r\n";
			client.sendToClient(joinMessage);
		}
		else
		{
			if (!it->isUserInChannel(&client))
			{
				it->addUser(&client);
				std::string joinMessage = ":" + client.getNick() + " JOIN " + channelName + "\r\n";
				it->broadcastMessage(joinMessage, &client);
				client.sendToClient(":" + client.getNick() + " JOIN " + channelName + "\r\n");
			}
			else
			{
				client.sendToClient(":server 443 " + client.getNick() + " " + channelName + " :is already on channel\r\n");
			}
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

void Command::privatmsg(std::vector<std::string> cmds, User &sender)
{
	if (!sender.getRegisteredStatus())
	{
		sender.sendToClient("451 :You have not registered\r\n");
		return;
	}

	if (cmds.size() < 3)
	{
		sender.sendToClient("412 :No text to send\r\n");
		return;
	}

	std::string target = cmds[1];
	std::string message = cmds[2];

	// Handle private messages to channels
	if (target[0] == '#')
	{
		auto it = std::find_if(channels.begin(), channels.end(), [&target](const Channel &channel)
							   { return channel.getName() == target; });

		if (it != channels.end())
		{
			if (it->isUserInChannel(&sender))
			{
				std::string formattedMessage = ":" + sender.getNick() + " PRIVMSG " + target + " :" + message + "\r\n";
				it->broadcastMessage(formattedMessage, &sender);
			}
			else
			{
				sender.sendToClient("404 " + target + " :Cannot send message to channel\r\n");
			}
		}
		else
		{
			sender.sendToClient("401 " + target + " :No such channel\r\n");
		}
	}
	else // Handle private messages to users
	{
		auto it = std::find_if(clients.begin(), clients.end(), [&target](const User &user)
							   { return user.getNick() == target; });

		if (it != clients.end())
		{
			std::string formattedMessage = ":" + sender.getNick() + " PRIVMSG " + target + " :" + message + "\r\n";
			it->sendToClient(formattedMessage);
		}
		else
		{
			sender.sendToClient("401 " + target + " :No such nick\r\n");
		}
	}
}

void Command::addChannel(const std::string &channelName)
{
	Channel newChannel(channelName);
	channels.push_back(newChannel);
}

bool Command::joinChannel(const std::string &channelName, User &user)
{
	for (auto &channel : channels)
	{
		if (channel.getName() == channelName)
		{
			channel.addUser(&user);
			return true; // Successfully joined the channel
		}
	}
	return false; // Channel not found
}

bool Command::leaveChannel(const std::string &channelName, User &user)
{
	auto it = std::find_if(channels.begin(), channels.end(), [&channelName](const Channel &channel)
						   { return channel.getName() == channelName; });

	if (it != channels.end())
	{
		it->removeUser(&user);
		return true; // Successfully left the channel
	}
	return false; // Channel not found
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

void Command::handleListCommand(const std::vector<std::string> &command, User &client)
{
	(void)command; // Unused parameter

	// Send the list of active channels to the client
	sendChannelList(client);
}

void Command::handleCreateCommand(const std::vector<std::string> &command, User &client)
{
	if (command.size() >= 2)
	{
		std::string channelName = command[1];

		// Ensure channel name starts with '#'
		if (channelName[0] != '#')
		{
			client.sendToClient(":server 403 " + client.getNick() + " " + channelName + " :Invalid channel name\r\n");
			return;
		}

		// Check if the channel already exists
		bool channelExists = std::any_of(channels.begin(), channels.end(), [&channelName](const Channel &channel)
										 { return channel.getName() == channelName; });

		if (!channelExists)
		{
			Channel newChannel(channelName);
			newChannel.addUser(&client);
			channels.push_back(newChannel);

			// Notify the client about channel creation
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

void Command::handleWhoisCommand(const std::vector<std::string> &command, User &client)
{
	if (command.size() >= 2)
	{
		std::string targetNick = command[1];

		auto targetUser = std::find_if(clients.begin(), clients.end(), [&targetNick](const User &user)
									   { return user.getNick() == targetNick; });

		if (targetUser != clients.end())
		{
			client.sendToClient("WHOIS " + targetNick + " " + targetUser->getIP() + " " + targetUser->getRealName() + "\n");

			client.sendToClient("End of WHOIS " + targetNick + "\n");
		}
		else
		{
			client.sendToClient(":server 401 " + client.getNick() + " " + targetNick + " :No such nick/channel\r\n");
		}
	}
	else
	{
		client.sendToClient(":server 461 " + client.getNick() + " WHOIS :Not enough parameters\r\n");
	}
}

void Command::handlePartCommand(const std::vector<std::string> &command, User &client)
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
				// Notify the client about the PART command
				std::string partMessage = ":" + client.getNick() + " PART " + channelName + " :Leaving channel\r\n";
				client.sendToClient(partMessage);

				// Notify other users in the channel about the departure
				std::string kickMessage = ":" + client.getNick() + " KICK " + channelName + " " + client.getNick() + " :You have been kicked from the channel\r\n";
				it->broadcastMessage(kickMessage, &client);

				// Remove the user from the channel
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
		client.sendToClient(":server 461 " + client.getNick() + " PART :Not enough parameters\r\n");
	}
}

void Command::handlePingCommand(const std::vector<std::string> &command, User &client)
{
	if (command.size() >= 2)
	{
		std::string pingMessage = "PONG :" + command[1] + "\r\n";
		client.sendToClient(pingMessage);
	}
	else
	{
		client.sendToClient(":server 461 " + client.getNick() + " PING :Not enough parameters\r\n");
	}
}

void Command::processRawClientData(const std::string &input, User &client)
{
	// Split the input into command and arguments
	(void)client;

	std::vector<std::string> command;
	size_t spacePos = input.find(' ');
	command.push_back(input.substr(0, spacePos));
	if (spacePos != std::string::npos)
	{
		command.push_back(input.substr(spacePos + 1));
	}

	std::cout << "Command::processRawClientData: raw input: " << input;
	std::cout << "Command::processRawClientData: command[0]: " << command[0] << std::endl;

	if (command[0] == "PING")
	{
		handlePingCommand(command, client);
	}
	else if (command[0] == "NICK") // irssi interpreteert /nick input als NICK en stuurt dat terug naar server
	{
		handleNickCommand(command, client);
	}
	if (command[0] == "PRIVMSG")
	{ // irssi interpreteert /msg input als PRIVMSG en stuurt dat terug naar server
		privatmsg(command, client);
	}
	// else if (command[0] == "AWAY")
	// {
	//     away(command, client);
	// }
	// else if (command[0] == "NOTICE")
	// {
	//     notice(command, client);
	// }
	// if (command[0] == "PASS")
	// {
	// 	handlePassCommand(command, client);
	// }
	else if (command[0] == "PART")
	{
		handlePartCommand(command, client);
	}

	// else if (command[0] == "user")
	// {
	// 	handleUserCommand(command, client);
	// }
	// else if (command[0] == "quit")
	// {
	// 	handleQuitCommand(command, client);
	// }
	else if (command[0] == "JOIN")
	{
		handleJoinCommand(command, client);
	}
	// else if (command[0] == "leave")
	// {
	// 	handleLeaveCommand(command, client);
	// }
	else if (command[0] == "LIST")
	{
		handleListCommand(command, client);
	}
	else if (command[0] == "CREATE")
	{
		handleCreateCommand(command, client);
	}
	// else
	// {
	// 	// Handle unknown command logic or send an error message
	// 	client.sendToClient(":server ERROR :Unknown command\r\n");
	// }
}
