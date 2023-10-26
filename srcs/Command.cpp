// Command.cpp

#include "./../incs/Command.hpp"
#include "./../incs/Client.hpp"
#include "./../incs/Channel.hpp"
#include <algorithm>
#include <unistd.h>
#include "./../incs/Server.hpp"

class Server; // Forward declaration of Server class

Command::Command(std::vector<Client> &clients, std::vector<Channel> &channels, Server &server)
	: clients(clients), channels(channels), ircServer(server)
{
	// Constructor implementation here
}

Command::~Command() {}

void Command::handlePassCommand(const std::vector<std::string> &command, Client &client)
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
			std::cout << "Client " << client.getNick() << " has successfully logged in." << std::endl;
		}
		else
		{
			client.sendToClient(":server 464 " + client.getNick() + " :Password incorrect\r\n");
			std::cout << "Client " << client.getNick() << " failed to log in. Incorrect password." << std::endl;
		}
	}
	else
	{
		client.sendToClient(":server 461 " + client.getNick() + " PASS :Not enough parameters\r\n");
	}
}

void Command::handleClientCommand(const std::vector<std::string> &command, Client &client)
{
	if (command.size() >= 5)
	{
		std::string clientname = command[1];
		std::string realname = command[4];

		client.setNick(clientname);
		client.setRealName(realname);

		client.sendToClient(":server 001 " + client.getNick() + " :Welcome to the IRC server, " + client.getNick() + "!\r\n");

		std::cout << "Client " << client.getNick() << " registered with clientname: " << clientname << " and real name: " << realname << std::endl;
	}
	else
	{
		client.sendToClient(":server 461 " + client.getNick() + " USER :Not enough parameters\r\n");
	}
}

void Command::handleQuitCommand(const std::vector<std::string> &command, Client &client)
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
	std::cout << "Client " << client.getNick() << " has quit the server. Reason: " << quitMessage << std::endl;

	// NOT WORKING

	// close(client.getSocketDescriptor());
	// auto iterator = std::find(clients.begin(), clients.end(), client);
	// if (iterator != clients.end())
	// 	clients.erase(iterator);

	// auto it = std::find_if(clients.begin(), clients.end(), [&client](const Client &client)
	// 					   { return client.getSocketDescriptor() == client.getSocketDescriptor(); });

	// if (it != clients.end())
	// {
	// 	clients.erase(it);
	// }

}

void Command::handleJoinCommand(const std::vector<std::string> &command, Client &client)
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
			newChannel.addClient(&client);
			channels.push_back(newChannel);

			// Notify the client about channel creation
			std::string joinMessage = ":" + client.getNick() + " JOIN " + channelName + "\r\n";
			client.sendToClient(joinMessage);
		}
		else
		{
			if (!it->isClientInChannel(&client))
			{
				it->addClient(&client);
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

void Command::privatmsg(std::vector<std::string> cmds, Client &sender)
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
			if (it->isClientInChannel(&sender))
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
	else // Handle private messages to clients
	{
		auto it = std::find_if(clients.begin(), clients.end(), [&target](const Client &client)
							   { return client.getNick() == target; });

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

bool Command::joinChannel(const std::string &channelName, Client &client)
{
	for (auto &channel : channels)
	{
		if (channel.getName() == channelName)
		{
			channel.addClient(&client);
			return true; // Successfully joined the channel
		}
	}
	return false; // Channel not found
}

bool Command::leaveChannel(const std::string &channelName, Client &client)
{
	auto it = std::find_if(channels.begin(), channels.end(), [&channelName](const Channel &channel)
						   { return channel.getName() == channelName; });

	if (it != channels.end())
	{
		it->removeClient(&client);
		return true; // Successfully left the channel
	}
	return false; // Channel not found
}

void Command::sendChannelList(Client &client)
{
	if (channels.empty())
	{
		client.sendToClient("There are no channels in this server.\n");
	}
	else
	{
		std::string channelList = "Channel List:\n";
		for (const auto &channel : channels)
		{
			channelList += channel.getName() + " (" + std::to_string(channel.getClientsCount()) + " clients)\n";
		}
		client.sendToClient(channelList);
	}
}

void Command::handleListCommand(const std::vector<std::string> &command, Client &client)
{
	(void)command; // Unused parameter

	// Send the list of active channels to the client
	sendChannelList(client);
}

void Command::handlePartCommand(const std::vector<std::string> &command, Client &client)
{
	std::cout << "Now inside Command::handlePartCommand()" << std::endl;

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

		if (it != channels.end())
		{
			if (it->isClientInChannel(&client))
			{
				// Debugging information
				std::cout << "Client " << client.getNick() << " is leaving channel " << channelName << std::endl;

				// Notify about leaving the channel
				std::string leaveMessage = ":" + client.getNick() + " PART " + channelName + " :Leaving the channel\r\n";
				it->broadcastMessage(leaveMessage, nullptr); // Broadcast to all clients in the channel

				// Remove client from the channel
				it->removeClient(&client);

				// If there are no clients left in the channel, remove the channel
				if (it->getClientsCount() == 0)
				{
					channels.erase(it);
					std::cout << "Channel " << channelName << " has been removed due to lack of clients." << std::endl;
				}
			}
			else
			{
				client.sendToClient(":server 442 " + client.getNick() + " " + channelName + " :You're not on that channel\r\n");
			}
		}
		else
		{
			client.sendToClient(":server 403 " + client.getNick() + " " + channelName + " :No such channel\r\n");
		}
	}
	else
	{
		client.sendToClient(":server 461 " + client.getNick() + " PART :Not enough parameters\r\n");
	}
}

void Command::processRawClientData(const std::string &input, Client &client)
{
	// Split the input into command and arguments
	// TODO MAKE NICE STRING SPLITTER?
	std::vector<std::string> command;
	size_t spacePos = input.find(' ');
	command.push_back(input.substr(0, spacePos));
	if (spacePos != std::string::npos)
	{
		command.push_back(input.substr(spacePos + 1));
	}

	std::cout << "Command::processRawClientData: raw input: " << input;
	std::cout << "Command::processRawClientData: command[0]: " << command[0] << std::endl;

	if (command[0] == "NICK") 										// irssi interpreteert /nick input als NICK en stuurt dat terug naar server
	{
		handleNickCommand(command, client);
	}
	else if (command[0] == "JOIN")												// irssi interpreteert /join input als JOIN en stuurt dat terug naar server
	{
		handleJoinCommand(command, client);
	}
	else if (command[0] == "PART")												// irssi interpreteert /part input (en /leave input) als PART en stuurt dat terug naar server
	{
		handlePartCommand(command, client);
	}

	else if (command[0] == "LIST")												// irssi interpreteert "/list -YES" als LIST en stuurt dat terug naar server
	{
		handleListCommand(command, client);
	}
	// if (command[0] == "PRIVMSG")
	// {
	// 	// irssi interpreteert /msg input als PRIVMSG en stuurt dat terug naar server
	// 	privatmsg(command, client);
	// }
}

/*
********************************************************************************
Command Handlers
********************************************************************************
*/

bool Command::isNicknameInUse(const std::string &nickname)
{
	for (const auto &client : clients)
	{
		if (client.getNick() == nickname)
		{
			return true;
		}
	}
	return false;
}

void Command::handleNickCommand(const std::vector<std::string> &command, Client &client)
{
	bool nicknameInUse;
	if (command.size() == 2)
	{
		std::string newNick = command[1];

		// Remove newline characters from the newNick string
		newNick.erase(std::remove(newNick.begin(), newNick.end(), '\n'), newNick.end());

		if (newNick == client.getNick())
		{
			client.sendToClient(":server 433 * " + newNick + " :Nickname is already in use\r\n");
		}
		// Check if newNick is not empty and different from the current nickname
		else if (!newNick.empty() && newNick != client.getNick())
		{
			nicknameInUse = this->isNicknameInUse(newNick);
			if (!nicknameInUse)
			{
				std::string oldNick = client.getNick(); // Get the current nickname before the change
				client.setNick(newNick);

				for (auto &channel : this->channels)
				{
					if (channel.isClientInChannel(&client))
					{
						std::string nickChangeMessage = ":" + oldNick + " NICK " + newNick + "\r\n";
						channel.broadcastMessage(nickChangeMessage, &client);
					}
				}
				// Inform the client about the nickname change
				std::string nickChangeResponse = ":" + oldNick + " NICK " + newNick + "\r\n";
				client.sendToClient(nickChangeResponse);

				std::cout << "Client " << oldNick << " has changed their nickname to " << newNick << std::endl;
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
