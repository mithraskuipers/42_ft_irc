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

#include <algorithm>
#include <cctype>

std::string sanitizeInput(const std::string &input)
{
	std::string sanitizedInput = input;
	// Remove all whitespace characters from the input string
	sanitizedInput.erase(std::remove_if(sanitizedInput.begin(), sanitizedInput.end(), ::isspace), sanitizedInput.end());
	return sanitizedInput;
}

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

		// Check if the nickname is already in use
		if (registeredNicknames.find(clientname) != registeredNicknames.end())
		{
			client.sendToClient(":server 433 * " + clientname + " :Nickname is already in use\r\n");
			return;
		}

		client.setNick(clientname);
		client.setRealName(realname);

		// Add the nickname to the registered nicknames set
		registeredNicknames.insert(clientname);

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

bool isChannelNameValid(const std::string &channelName)
{

	if (channelName.length() < 4 || channelName.length() > 22)
	{
		return false;
	}

	// Kijk of eerste # is. Automatisch gemaakt door irssi.
	if (channelName[0] != '#')
	{
		return false;
	}

	// Sla eerste over. Sla laatste twee over. Alles tussenin moet alfanumeriek.
	if (!std::all_of(channelName.begin() + 1, channelName.end() - 2, [](unsigned char c)
					 { return std::isalnum(c); }))
	{
		return false;
	}

	// Kijk of laatste twee \r\n zijn.
	if (channelName.substr(channelName.length() - 2) != "\r\n")
	{
		return false;
	}

	return true;
}

void Command::handleJoinCommand(const std::vector<std::string> &command, Client &client)
{
	if (command.size() >= 2)
	{
		std::string channelName = command[1];
		std::cout << channelName << std::endl;
		std::cout << channelName.size() << std::endl;

		if (!isChannelNameValid(channelName))
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
            channelList += channel.getName() + " (" + std::to_string(channel.getClientsCount()) + " clients): ";

            // Add nicknames of clients in the channel to the list
            for (const auto &client : channel.getClients())
            {
                channelList += client->getNick() + " ";
            }

            channelList += "\n";
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

#include <sstream>

std::vector<std::string> Command::parseRawData(const std::string &data)
{
    std::vector<std::string> result;
    std::istringstream iss(data);
    std::string token;
    while (std::getline(iss, token, ' '))
    {
        result.push_back(token);
    }
    return result;
}

Channel* Command::findChannelByName(const std::string &channelName)
{
    auto it = std::find_if(channels.begin(), channels.end(), [&channelName](const Channel &channel)
    {
        return channel.getName() == channelName;
    });

    if (it != channels.end())
    {
        return &(*it);
    }

    return nullptr; // Return nullptr if channel is not found
}

void Command::removeChannel(Channel* channel)
{
    auto it = std::remove_if(channels.begin(), channels.end(), [channel](const Channel &c)
    {
        return &c == channel;
    });

    channels.erase(it, channels.end()); // Erase the channel from the vector
}



void Command::handlePartCommand(Client *client, const std::vector<std::string> &command) {
    if (command.size() >= 2) {
        std::string channelName = command[1];

        if (channelName[0] != '#') {
            // Invalid channel name, send an error message to the client
            std::string errorMessage = "403 " + client->getNick() + " " + channelName + " :Invalid channel name\r\n";
            client->send(errorMessage);
        } else {
            Channel *channel = findChannelByName(channelName);
            if (channel != nullptr) {
                if (channel->isClientInChannel(client)) {
                    // Inform about leaving the channel
                    std::string leaveMessage = ":" + client->getNick() + " PART " + channelName + " :Leaving the channel\r\n";
                    channel->broadcastMessage(leaveMessage, client);

                    // Remove client from the channel
                    channel->removeClient(client);

                    // If there are no clients left in the channel, remove the channel
                    if (channel->isEmpty()) {
                        // Remove the channel from the list of channels
                        removeChannel(channel);
                    }
                } else {
                    // Client is not in the channel, send an error message to the client
                    std::string errorMessage = "442 " + client->getNick() + " " + channelName + " :You're not on that channel\r\n";
                    client->send(errorMessage);
                }
            } else {
                // Channel not found, send an error message to the client
                std::string errorMessage = "403 " + client->getNick() + " " + channelName + " :No such channel\r\n";
                client->send(errorMessage);
            }
        }
    } else {
        // Invalid PART command, send an error message to the client
        std::string errorMessage = "461 " + client->getNick() + " PART :Not enough parameters\r\n";
        client->send(errorMessage);
    }
}



// void Command::processRawClientData(const std::string &input, Client &client)
// {
// 	// Split the input into command and arguments
// 	// TODO MAKE NICE STRING SPLITTER?
// 	std::vector<std::string> command;
// 	size_t spacePos = input.find(' ');
// 	command.push_back(input.substr(0, spacePos));
// 	if (spacePos != std::string::npos)
// 	{
// 		command.push_back(input.substr(spacePos + 1));
// 	}

// 	std::cout << "Command::processRawClientData: raw input: " << input;
// 	std::cout << "Command::processRawClientData: command[0]: " << command[0] << std::endl;

// 	if (command[0] == "NICK") // irssi interpreteert /nick input als NICK en stuurt dat terug naar server
// 	{
// 		handleNickCommand(command, client);
// 	}
// 	else if (command[0] == "JOIN") // irssi interpreteert /join input als JOIN en stuurt dat terug naar server
// 	{
// 		handleJoinCommand(command, client);
// 	}
// 	else if (command[0] == "PART") // irssi interpreteert /part input (en /leave input) als PART en stuurt dat terug naar server
// 	{
// 		handlePartCommand(command, client);
// 	}

// 	else if (command[0] == "LIST") // irssi interpreteert "/list -YES" als LIST en stuurt dat terug naar server
// 	{
// 		handleListCommand(command, client);
// 	}
// 	// if (command[0] == "PRIVMSG")
// 	// {
// 	// 	// irssi interpreteert /msg input als PRIVMSG en stuurt dat terug naar server
// 	// 	privatmsg(command, client);
// 	// }
// }





void Command::processRawClientData(const std::string &data, Client *client) {
    // Parse the command and arguments from the raw data
    std::vector<std::string> commandAndArgs = parseRawData(data);

    // Check the command type and perform appropriate actions
    if (commandAndArgs[0] == "PART") {
        // Handle PART command
        if (commandAndArgs.size() > 1) {
            std::string channelName = commandAndArgs[1];
            Channel *channel = findChannelByName(channelName);
            if (channel != nullptr) {
                // Check if the client is in the channel
                if (channel->isClientInChannel(client)) {
                    // Inform about leaving the channel
                    std::string leaveMessage = ":" + client->getNick() + " PART " + channelName + " :Leaving the channel\r\n";
                    channel->broadcastMessage(leaveMessage, client);

                    // Remove client from the channel
                    channel->removeClient(client);

                    // If there are no clients left in the channel, remove the channel
                    if (channel->isEmpty()) {
                        // Remove the channel from the list of channels
                        removeChannel(channel);
                    }
                } else {
                    // Client is not in the channel, send an error message to the client
                    std::string errorMessage = "442 " + client->getNick() + " " + channelName + " :You're not on that channel\r\n";
                    client->send(errorMessage);
                }
            } else {
                // Channel not found, send an error message to the client
                std::string errorMessage = "403 " + client->getNick() + " " + channelName + " :No such channel\r\n";
                client->send(errorMessage);
            }
        } else {
            // Invalid PART command, send an error message to the client
            std::string errorMessage = "461 " + client->getNick() + " PART :Not enough parameters\r\n";
            client->send(errorMessage);
        }
    }

    // ... (other command handling logic)
}



/*
********************************************************************************
Command Handlers
********************************************************************************
*/

bool Command::isNicknameInUse(const std::string &nickname)
{
	return registeredNicknames.find(nickname) != registeredNicknames.end();
}

// Reused isChannelnameValid() func. Please turn it into a single re-used function called for multiple purposes
// Note the pointer arithmetic for skipping first char in channelname usecase.
bool Command::isNicknameValid(const std::string &nickname)
{

	if (nickname.length() < 4 || nickname.length() > 22)
	{
		return false;
	}

	// Sla eerste over. Sla laatste twee over. Alles tussenin moet alfanumeriek.
	if (!std::all_of(nickname.begin() + 0, nickname.end() - 2, [](unsigned char c)
					 { return std::isalnum(c); }))
	{
		return false;
	}

	// Kijk of laatste twee \r\n zijn.
	if (nickname.substr(nickname.length() - 2) != "\r\n")
	{
		return false;
	}

	return true;
}


/*
Belangrijk!
De isNicknameValid() is belangrijk omdat het kijkt of de nickname volledig alphanumeric is.
Als dat het niet is, dan rejecten we de nickname change. Waarom? Omdat we anders in de backend
opslaan dat de nickname non-alphanumeric characters in zich heeft. Dit zorgt voor een out of sync
probleem omdat irssi zelf automatisch de non-alphanumeric characters van de nickname lijkt te verwijderen.
Dit probleem heeft dus weer te maken met een verschil tussen wat je als user invoert en de vertaalslag van
irssi naar onze server.
*/

void Command::handleNickCommand(const std::vector<std::string> &command, Client &client)
{
	if (command.size() == 2)
	{

		std::string newNick = command[1]; // Sanitize the input

		newNick = sanitizeInput(newNick); // Sanitize the input


		if (!isNicknameValid(command[1]))
		{
			client.sendToClient(":server 432 * :Erroneous Nickname\r\n");
			return;
		}
		// std::string newNick = command[1];
		if (DEBUG)
		{
			std::cout << command[1] << std::endl;
			std::cout << command[1].size() << std::endl;
		}
		if (DEBUG)
		{
			std::cout << newNick << std::endl;
			std::cout << newNick.size() << std::endl;
		}


		if (!newNick.empty())
		{
			if (!isNicknameInUse(newNick))
			{
				std::string oldNick = client.getNick(); // Get the current nickname before the change
				if (DEBUG)
				{
					std::cout << oldNick << std::endl;
					std::cout << oldNick.size() << std::endl;
				}	
				client.setNick(newNick);
				if (DEBUG)
				{
					std::cout << client.getNick() << std::endl;
					std::cout << client.getNick().size() << std::endl;
				}

				// Broadcast the nickname change to all channels
				std::string nickChangeMessage = ":" + oldNick + " NICK " + newNick + "\r\n";
				for (auto &channel : this->channels)
				{
					if (channel.isClientInChannel(&client))
					{
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
