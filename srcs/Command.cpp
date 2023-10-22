/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/21 20:49:19 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/22 15:34:46 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

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

void Command::process(const std::string &input, User &client)
{
	// Split the input into command and arguments
	std::vector<std::string> command;
	size_t spacePos = input.find(' ');
	command.push_back(input.substr(0, spacePos)); // Extract the command

	if (spacePos != std::string::npos)
	{
		command.push_back(input.substr(spacePos + 1)); // Extract the arguments
	}
	else
	{
		// No space found, handle this case (send an error message, etc.)
		client.sendToClient(":server ERROR :Invalid command format\r\n");
		return;
	}

	std::cout << "[DEBUG] [root]: input received: " << input << std::endl;
	std::cout << "[DEBUG] [command[0]]: input received: " << command[0] << std::endl;
	std::cout << "[DEBUG] [command[1]]: input received: " << command[1] << std::endl;

	if (command[0] == "pass")
	{

		std::cout << "command.size():" << command.size() << std::endl;
		std::string password = command[1];
		std::cout << "[DEBUG] [/pass]: pass received: " << password << std::endl;

		std::cout << "[DEBUG] [/pass]: server pass: " << ircServer.getPass() << std::endl;

		std::cout << "Received Password Length: " << password.length() << std::endl;
		std::cout << "Server Password Length: " << ircServer.getPass().length() << std::endl;

		// Trim the received password to match the length of the server's password
		password = password.substr(0, ircServer.getPass().length());

		// Compare the provided password with the server password
		if (password == ircServer.getPass())
		{
			std::cout << "YOUR PASSWORD MATCHES THE SERVED PASSWORD!!!!" << std::endl;
			// Set client registered status to true
			client.setRegistered(true);
			// Send success message to the client
			std::string successMessage = ":server 001 " + client.getNick() + " :You have successfully logged in, " + client.getNick() + "!\r\n";
			client.sendToClient(successMessage);
			// You can also perform additional actions after successful login if needed
			std::cout << "User " << client.getNick() << " has successfully logged in." << std::endl;
		}
		else
		{
			// Send error message to the client for incorrect password
			client.sendToClient(":server 464 " + client.getNick() + " :Password incorrect\r\n");
			std::cout << "User " << client.getNick() << " failed to log in. Incorrect password." << std::endl;
		}
	}
	else if (command[0] == "nick")
	{
		std::string newNick;
		if (spacePos != std::string::npos)
		{
			newNick = command[1];

			if (!newNick.empty())
			{
				// Check if the new nickname is already in use
				bool nicknameInUse = false;
				for (const auto &channel : channels)
				{
					for (const auto &user : channel.getUsers())
					{
						if (user->getNick() == newNick)
						{
							nicknameInUse = true;
							break;
						}
					}
				}

				if (!nicknameInUse)
				{
					std::string oldNick = client.getNick();
					client.setNick(newNick);

					// Broadcast the nick change to all channels the user is in
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

					// Send custom server response after nickname change
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

	else if (command[0] == "quit")
	{
		std::string quitMessage = input.substr(spacePos + 1);

		// Broadcast quit message to all channels the user is in
		// Inside the function, you can now use the 'clients' vector
		auto iterator = std::find_if(clients.begin(), clients.end(), [&client](const User &user)
									 { return user.getSocketDescriptor() == client.getSocketDescriptor(); });

		if (iterator != clients.end())
		{
			clients.erase(iterator);
		}

		// Close the client socket and remove the user from the server
		close(client.getSocketDescriptor());

		// Find the client in the clients vector and remove it
		auto it = std::find_if(clients.begin(), clients.end(), [&client](const User &user)
							   { return user.getSocketDescriptor() == client.getSocketDescriptor(); });
		if (it != clients.end())
		{
			clients.erase(it);
		}

		std::cout << "User " << client.getNick() << " has quit the server." << std::endl;
	}

	else if (command[0] == "user")
	{
		std::string parameters = input.substr(spacePos + 1);
		size_t secondSpacePos = parameters.find(' ');

		if (secondSpacePos != std::string::npos)
		{
			std::string username = parameters.substr(0, secondSpacePos);
			std::string realname = parameters.substr(secondSpacePos + 1);

			// Set the user's username and real name
			client.setNick(username);
			client.setRealName(realname);

			// Respond with a welcome message or MOTD (Message of the Day)
			client.sendToClient(":server 001 " + client.getNick() + " :Welcome to the IRC server, " + client.getNick() + "!\r\n");

			// You can add more welcome messages or instructions here if needed

			std::cout << "User " << client.getNick() << " registered with username: " << username << " and real name: " << realname << std::endl;
		}
		else
		{
			client.sendToClient(":server 461 " + client.getNick() + " USER :Not enough parameters\r\n");
			// You can send an error response indicating incorrect usage of the USER command
		}
	}

	else if (command[0] == "list")
	{
		std::cout << "Executing /list for " << client.getNick() << std::endl;
		sendChannelList(client);
		return;
	}
	else if (command[0] == "create")
	{
		std::string channelName = input.substr(spacePos + 1);
		bool channelExists = false;

		// Check if the channel already exists
		for (const auto &channel : channels)
		{
			if (channel.getName() == channelName)
			{
				channelExists = true;
				break;
			}
		}

		if (!channelExists)
		{
			Channel newChannel(channelName);
			newChannel.addUser(&client);
			channels.push_back(newChannel);
			std::cout << "Created new channel named: " << channelName << std::endl;

			// Send IRC protocol messages for channel creation
			std::string joinMessage = ":" + client.getNick() + " JOIN :" + channelName + "\r\n";
			std::string topicMessage = ":" + client.getNick() + " TOPIC " + channelName + " :Welcome to " + channelName + " channel!\r\n";
			std::string endOfNamesMessage = ":server 366 " + client.getNick() + " " + channelName + " :End of NAMES list\r\n";

			client.sendToClient("Channel created: " + channelName + "\n");

			// Send IRC messages to all users in the new channel
			for (auto &user : channels.back().getUsers())
			{
				user->sendToClient(joinMessage);
				user->sendToClient(topicMessage);
				user->sendToClient(endOfNamesMessage);
			}
		}
		else
		{
			// Send error message to the client if the channel already exists
			client.sendToClient(":server 403 " + client.getNick() + " " + channelName + " :Channel already exists\r\n");
			std::cout << "Channel already exists with the name: " << channelName << std::endl;
		}
	}

	else if (command[0] == "join")
	{
		std::string channelName = input.substr(spacePos + 1);
		bool alreadyInChannel = false;
		bool channelFound = false;

		// Check if the client is already in a channel
		for (const auto &channel : channels)
		{
			if (channel.isUserInChannel(&client))
			{
				alreadyInChannel = true;
				break;
			}
		}

		if (!alreadyInChannel)
		{
			for (auto &channel : channels)
			{
				if (channel.getName() == channelName)
				{
					// Add the user to the channel
					channel.addUser(&client);
					client.sendToClient("Joined channel: " + channelName + "\n");

					// Send JOIN message back to the client indicating successful channel join
					std::string joinMessage = ":" + client.getNick() + " JOIN " + channelName + "\n";
					client.sendToClient(joinMessage);

					// Send the channel topic and user list (if available) here, if needed

					channelFound = true;
					break;
				}
			}

			if (!channelFound)
			{
				client.sendToClient("Channel not found: " + channelName + "\n");
			}
		}
		else
		{
			client.sendToClient("You are already in the channel " + channelName + ".\n");
		}
	}

	else if (command[0] == "leave")
	{
		std::string channelName = input.substr(spacePos + 1);
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
}
