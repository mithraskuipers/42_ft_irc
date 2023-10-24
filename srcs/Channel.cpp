/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/21 20:49:24 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/24 15:45:29 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Channel.hpp"
#include "./../incs/User.hpp"
#include "./../incs/includes.hpp"

Channel::Channel() : topic(""), topicAuthor("") {}

Channel::~Channel() {}

Channel::Channel(const std::string &name) : channelName(name), topic(""), topicAuthor("") {}

void Channel::addUser(User *user)
{
	users.push_back(user);
}

void Channel::removeUser(User *user)
{
	auto it = std::remove(users.begin(), users.end(), user);
	users.erase(it, users.end());
}

bool Channel::isUserInChannel(const User *user) const
{
	return std::find(users.begin(), users.end(), user) != users.end();
}

void Channel::broadcastMessage(const std::string &message, User *sender)
{
	for (auto &user : users)
	{
		if (user != sender)
		{
			user->sendToClient(message);
		}
	}
}

std::string Channel::getName() const
{
	return channelName;
}

void Channel::setTopic(const std::string &newTopic, User *user)
{
	topic = newTopic;
	topicAuthor = user->getNick();
}

std::string Channel::getTopic() const
{
	return topic;
}

std::string Channel::getTopicAuthor() const
{
	return topicAuthor;
}

int Channel::getUsersCount() const
{
	return users.size();
}

bool Channel::isEmpty() const
{
	return users.empty();
}

#include <string> // Include the necessary header for std::string

// Assuming USER_IDENTIFIER is a std::string
std::string USER_IDENTIFIER = "User:";

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

void Channel::listUsers() const
{
	std::cout << "Users currently in the channel " << channelName << ":\n";
	for (const auto &user : users)
	{
		std::cout << "- " << user->getNick() << "\n";
	}
}
