/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/21 20:49:19 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/21 23:57:49 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// Command.cpp

#include "Command.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <algorithm>
#include <unistd.h>


Command::Command(std::vector<User>& clients) : clients(clients)
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
	            // Check if the nickname is already in use
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
	                if (client.getNick().empty())
	                {
	                    client.setNick(newNick);
	                    client.sendToClient(":" + newNick + " NICK " + newNick + "\r\n");
	                    std::cout << "Your nickname has been set to: " << newNick << std::endl;
	                }
	                else
	                {
	                    std::string oldNick = client.getNick();
	                    client.setNick(newNick);
	                    client.sendToClient(":" + oldNick + " NICK " + newNick + "\r\n");
	                    std::cout << "Your nickname has been changed from " << oldNick << " to " << newNick << std::endl;
	                }
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
	else if (command == "/quit")
	{
	    std::string quitMessage = input.substr(spacePos + 1);
	
	    // Broadcast quit message to all channels the user is in
    	// Inside the function, you can now use the 'clients' vector
		auto iterator = std::find_if(clients.begin(), clients.end(), [&client](const User &user) {
		    return user.getSocketDescriptor() == client.getSocketDescriptor();
		});
		
		if (iterator != clients.end()) {
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
	
	else if (command == "/user")
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
			client.sendToClient("Channel already exists with the name: " + channelName + "\n");
		}
	}

	else if (command == "/join")
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
