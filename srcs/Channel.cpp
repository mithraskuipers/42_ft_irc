/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/21 20:49:24 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/21 20:52:49 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "User.hpp"
#include "includes.hpp"

Channel::Channel() : topic(""), topicAuthor("") {}

Channel::~Channel() {}

Channel::Channel(const std::string &name) : channelName(name) {}

void Channel::addUser(User *user)
{
	users.push_back(user);
}



int Channel::getUsersCount() const
{
	return users.size();
}

bool Channel::isEmpty() const
{
	return users.empty();
}

std::string Channel::getName() const
{
	return channelName;
}

#include <string> // Include the necessary header for std::string

// Assuming USER_IDENTIFIER is a std::string
std::string USER_IDENTIFIER = "User:";

// Inside your setTopic function
void Channel::setTopic(const std::string &topic, User *user)
{
	// Assuming getNick() returns a C-style string
	topicAuthor = USER_IDENTIFIER + " " + std::string(user->getNick());
	(void)topic;
	// Rest of your code
}

std::string Channel::getTopic() const
{
	return topic;
}

std::string Channel::getTopicAuthor() const
{
	return topicAuthor;
}

bool Channel::isOperator(User *user) const
{
	auto it = std::find(operators.begin(), operators.end(), user);
	return it != operators.end();
}

void Channel::addOperator(User *user)
{
	operators.push_back(user);
}

void Channel::removeOperator(User *user)
{
	auto it = std::find(operators.begin(), operators.end(), user);
	if (it != operators.end())
	{
		operators.erase(it);
	}
}


void Channel::listUsers() const {
    std::cout << "Users currently in the channel " << channelName << ":\n";
    for (const auto& user : users) {
        std::cout << "- " << user->getNick() << "\n";
    }
}


bool Channel::isUserInChannel(const User* user) const {
	    std::cout << "Checking if user is in the channel..." << std::endl;
		listUsers();
        for (const auto& u : users) {
            if (u == user) {
                return true;
            }
        }
        return false;
}

void Channel::removeUser(User *user)
{
	    std::cout << "Removing user from the channel..." << std::endl;
		listUsers();
    auto it = std::find(users.begin(), users.end(), user);
    if (it != users.end())
    {
        users.erase(it);
    }
}
