#include "../incs/includes.hpp"
#include "../incs/user.hpp"
#include <sstream>
#include "../incs/channel.hpp"



user::user(int socket_fd) : _socket(socket_fd)
{
    // Initialize other member variables if needed
}

user::~user()
{
    // Perform cleanup if necessary
}

void user::commandHandler(const std::string& buffer, std::list<channel>& channelList)
{
    // Assuming commands are in the format: /COMMAND ARGUMENTS...
    std::istringstream iss(buffer);
    std::string command;
    iss >> command; // Extract the command (e.g., /JOIN, /CREATE, /LIST)

    if (command == "/JOIN") {
        std::string channelName;
        iss >> channelName; // Extract the channel name from the command arguments

        // Find the channel in the channelList
        auto it = std::find_if(channelList.begin(), channelList.end(), [&channelName](const channel& c) {
            return c.getName() == channelName;
        });

        if (it != channelList.end()) {
            // Channel found, add the user to the channel
            it->addUser(*this);
        } else {
            // Channel not found, handle accordingly (e.g., send an error message to the user)
        }
    } else if (command == "/CREATE") {
        std::string channelName;
        iss >> channelName; // Extract the channel name from the command arguments

        // Check if the channel with the same name already exists
        auto it = std::find_if(channelList.begin(), channelList.end(), [&channelName](const channel& c) {
            return c.getName() == channelName;
        });

        if (it == channelList.end()) {
            // Channel does not exist, create a new channel and add the user to it
            channel newChannel(channelName);
            newChannel.addUser(*this);
            channelList.push_back(newChannel);
        } else {
            // Channel with the same name already exists, handle accordingly (e.g., send an error message to the user)
        }
    } else if (command == "/LIST") {
        // Prepare a message with the list of available channels and send it to the user
        std::string message = "Channels: ";
        for (const auto& channel : channelList) {
            message += channel.getName() + " ";
        }
        message += "\n";

        // Send the message to the user
        send(this->_socket, message.c_str(), message.size(), 0);
    } else {
        // Unknown command, handle accordingly (e.g., send an error message to the user)
    }
}

void user::setName(const std::string& username)
{
    this->_username = username;
}

void user::setPass(const std::string& password)
{
    this->_password = password;
}

std::string user::getName() const
{
    return this->_username;
}

std::string user::getPass() const
{
    return this->_password;
}

int user::getSocket() const
{
    return this->_socket;
}

void user::setSocket(int socket_fd)
{
    this->_socket = socket_fd;
}

int user::getID() const
{
    return this->_id;
}

void user::setID(int id)
{
    this->_id = id;
}
