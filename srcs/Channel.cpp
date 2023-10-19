/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/19 12:44:44 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/19 17:25:09 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Channel.hpp"
#include "./../incs/User.hpp"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <sys/socket.h> // Add this line to include the necessary header

Channel::Channel(std::string channelName) : _channelName(channelName) {}

Channel::~Channel() {}

void Channel::addUser(User newUser)
{
    _channelMembers.push_back(newUser);
    std::string message = "You have joined Channel: " + this->_channelName + "\n";
    send(newUser.getSocketDescriptor(), message.c_str(), std::strlen(message.c_str()), 0);
    std::cout << newUser.getNick() << " added to Channel: " << this->_channelName << std::endl;
}

void Channel::removeUser(User oldUser)
{
    auto it = std::remove_if(_channelMembers.begin(), _channelMembers.end(), [&](const User& user) {
        return user.getNick() == oldUser.getNick();
    });
    _channelMembers.erase(it, _channelMembers.end());
}

void Channel::broadcastMessage(const std::string& message, User sender)
{
    for (const auto& user : _channelMembers)
    {
        if (user.getNick() != sender.getNick()) // Don't send the message back to the sender
        {
            std::string fullMessage = sender.getNick() + ": " + message + "\n";
            send(user.getSocketDescriptor(), fullMessage.c_str(), std::strlen(fullMessage.c_str()), 0);
        }
    }
}

std::string Channel::getName() const
{
    return this->_channelName;
}
