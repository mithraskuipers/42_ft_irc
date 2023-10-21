/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/21 20:49:19 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/21 22:09:31 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// Command.cpp

#include "Command.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <algorithm>

Command::Command()
{
	// Constructor implementation
}

Command::~Command()
{
	// Destructor implementation
}

void Command::sendChannelList(User &user, const std::vector<Channel> &channels)
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

void Command::process(const std::string &input, User &client, std::vector<Channel> &channels)
{
	size_t spacePos = input.find(' ');
	std::string command = input.substr(0, spacePos); // Extract the command

	if (command == "/nick")
	{
		std::string newNick;
		if (spacePos != std::string::npos)
		{
			newNick = input.substr(spacePos + 1);
			newNick = newNick.substr(newNick.find_first_not_of(' '));

			if (!newNick.empty())
			{
				if (client.getNick().empty())
				{
					client.setNick(newNick);
					std::cout << "Your nickname has been set to: " << newNick << std::endl;
				}
				else
				{
					std::string oldNick = client.getNick();
					client.setNick(newNick);
					std::cout << "Your nickname has been changed from " << oldNick << " to " << newNick << std::endl;
				}
			}
			else
			{
				std::cout << "Invalid nickname. Please try again." << std::endl;
			}
		}
	}
	else if (command == "/list")
	{
		std::cout << "Executing /list for " << client.getNick() << std::endl;
		sendChannelList(client, channels);
		return;
	}
	else if (command == "/create")
	{
		std::string channelName = input.substr(spacePos + 1);
		bool channelExists = false;
		for (const auto &channel : channels)
		{
			if (channel.getName() == channelName)
			{
				channelExists = true;
				std::cout << "Channel already exists with the name: " << channelName << std::endl;
				break;
			}
		}
		if (!channelExists)
		{
			Channel newChannel(channelName);
			newChannel.addUser(&client);
			channels.push_back(newChannel);
			std::cout << "Created new channel named: " << channelName << std::endl;
		}
	}

	else if (command == "/join")
	{
		std::string channelName = input.substr(spacePos + 1);
		bool alreadyInChannel = false;
		bool channelFound = false;

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
					channel.addUser(&client);
					client.sendToClient("Joined channel: " + channelName + "\n");
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

	else if (command == "/leave")
	{
		std::string channelName = input.substr(spacePos + 1);
		auto it = std::find_if(channels.begin(), channels.end(), [&channelName](const Channel &channel)
							   { return channel.getName() == channelName; });

		if (it != channels.end())
		{
			if (it->isUserInChannel(&client))
			{
				it->removeUser(&client);
				std::cout << "You left channel: " << channelName << std::endl;
			}
			else
			{
				std::cout << "You are not in channel: " << channelName << std::endl;
			}
		}
		else
		{
			std::cout << "Channel not found: " << channelName << std::endl;
		}
	}
}
