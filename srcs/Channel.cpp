/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/21 20:49:24 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/24 22:31:50 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Channel.hpp"
#include "./../incs/Client.hpp"
#include "./../incs/includes.hpp"
#include <string> // Include the necessary header for std::string

/*
********************************************************************************
Orthodox canonical form
********************************************************************************
*/

Channel::Channel() : topic(""), topicAuthor("")
{
}

Channel::~Channel()
{
}

Channel::Channel(const std::string &name) : channelName(name), topic(""), topicAuthor("")
{
}

/*
********************************************************************************
Getters
********************************************************************************
*/

std::string Channel::getName() const
{
	return channelName;
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

/*
********************************************************************************
Setters
********************************************************************************
*/

void Channel::setTopic(const std::string &newTopic, Client *user)
{
	topic = newTopic;
	topicAuthor = user->getNick();
}

/*
********************************************************************************
Member functions
********************************************************************************
*/

void Channel::addUser(Client *user)
{
	users.push_back(user);
}

void Channel::removeUser(Client *user)
{
	auto it = std::remove(users.begin(), users.end(), user);
	users.erase(it, users.end());
}

bool Channel::isUserInChannel(const Client *user) const
{
	return std::find(users.begin(), users.end(), user) != users.end();
}

void Channel::broadcastMessage(const std::string &message, Client *sender)
{
	for (auto &user : users)
	{
		if (user != sender)
		{
			user->sendToClient(message);
		}
	}
}

bool Channel::isEmpty() const
{
	return users.empty();
}

bool Channel::isOperator(Client *user) const
{
	auto it = std::find(operators.begin(), operators.end(), user);
	return it != operators.end();
}

void Channel::addOperator(Client *user)
{
	operators.push_back(user);
}

void Channel::removeOperator(Client *user)
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
