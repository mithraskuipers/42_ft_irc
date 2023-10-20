/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Channel.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/19 13:48:42 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/20 09:19:26 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "./User.hpp" // Include the User class for User objects

class Channel {
public:
	Channel(std::string channelName);
	~Channel();

	void addUser(User newUser);
	void removeUser(User oldUser);
	void broadcastMessage(const std::string& message, User sender);
	std::string getName() const;

private:
	std::string _channelName;
	std::vector<User> _channelMembers;
};

#endif
