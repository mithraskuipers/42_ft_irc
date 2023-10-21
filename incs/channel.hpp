/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   channel.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/20 14:27:34 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/20 14:45:42 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "includes.hpp"
#include <netinet/in.h> // struct sockaddr_in

class user; // Forward declaration of the user class

class channel
{
	public:
		channel(); // default constructor
        channel(std::string channelName);
		~channel(); // destructor

		channel &operator=(const channel &src);
        void setName(std::string);
        std::string getName() const;
        void setUserCount(int userCount) { this->_userCount = userCount; }
        int getUserCount() { return this->_userCount;}
        user *getChannelMembers() { return this->_channelMembers;}
        void addUser(user newUser);
        void removeUser(user oldUser);        

	private:
        std::string _channelName;
        user        *_channelMembers;
        int         _userCount;


};

#endif