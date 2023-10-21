/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   channel.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/20 14:27:27 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/20 14:34:58 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/channel.hpp"

channel::channel()
{
    this->_channelMembers = new user[100];
    _userCount = 0;
}

channel::channel(std::string channelName) : _channelName(channelName)
{
    this->_channelMembers = new user[100];
    _userCount = 0;
}

channel::~channel()
{

}


channel &channel::operator=(const channel &src)
{
	std::cout << "channel Assignation operator called" << std::endl;
	if (this == &src)
		return *this;
	this->_channelName = src._channelName;
	return *this;
}

void channel::addUser(user newUser)
{
    this->_channelMembers[this->_userCount].setName(newUser.getName());
    this->_channelMembers[this->_userCount].setSocket(newUser.getSocket());
    this->_channelMembers[this->_userCount].setID(newUser.getID());

    this->_userCount++;
    std::string message = "You have joined channel: " + this->_channelName + "\n";
    send(newUser.getSocket(), message.c_str(), strlen(message.c_str()), 0);
    std::cout << newUser.getName() << " added to channel: " << this->_channelName << std::endl;
}

void channel::removeUser(user oldUser)
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

// void channel::createChannel(std::string channelName, int channelCount)
// {
//     if (channelCount >= 10)
//     {
//         std::cout << "Max channels reached" << std::endl;
//         return;
//     }
//     this->setName(channelName);
//     channelCount++;
// }

void channel::setName(std::string channelname)
{
    this->_channelName = channelname;
}

std::string channel::getName() const {
    return _channelName;
}
