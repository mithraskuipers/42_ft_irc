/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/19 12:44:44 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/19 12:48:07 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Channel.hpp"
#include "./../incs/User.hpp"


Channel::Channel()
{
    this->_channelMembers = new User[100];
    _userCount = 0;
}

Channel::Channel(std::string channelName) : _channelName(channelName)
{
    this->_channelMembers = new User[100];
    _userCount = 0;
}

Channel::~Channel()
{

}

Channel &Channel::operator=(const Channel &src)
{
	std::cout << "Channel Assignation operator called" << std::endl;
	if (this == &src)
		return *this;
	this->_channelName = src._channelName;
	return *this;
}

void Channel::addUser(User newUser)
{
    this->_channelMembers[this->_userCount].setName(newUser.getName());
    this->_channelMembers[this->_userCount].setSocket(newUser.getSocket());
    this->_channelMembers[this->_userCount].setid(newUser.getid());
    this->_userCount++;
    std::string message = "You have joined Channel: " + this->_channelName + "\n";
    send(newUser.getSocket(), message.c_str(), strlen(message.c_str()), 0);
    std::cout << newUser.getName() << " added to Channel: " << this->_channelName << std::endl;
}

void Channel::removeUser(User oldUser)
{
    int i = 0;
    while (i < this->_userCount)
    {
        if (this->_channelMembers[i].getName() == oldUser.getName())
        {
            while (i < this->_userCount)
            {
                this->_channelMembers[i] = this->_channelMembers[i + 1];
                i++;
            }
            this->_userCount--;
            return;
        }
        i++;
    }
}

// void Channel::createChannel(std::string channelName, int channelCount)
// {
//     if (channelCount >= 10)
//     {
//         std::cout << "Max channels reached" << std::endl;
//         return;
//     }
//     this->setName(channelName);
//     channelCount++;
// }

void Channel::setName(std::string channelname)
{
    this->_channelName = channelname;
}

std::string Channel::getName()
{
    return this->_channelName;
}
