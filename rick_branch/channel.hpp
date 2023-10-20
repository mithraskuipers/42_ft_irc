#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "includes.hpp"
#include <netinet/in.h> // struct sockaddr_in

class user;


class channel
{
	public:
		channel(); // default constructor
        channel(std::string channelName);
		~channel(); // destructor

		channel &operator=(const channel &src);
        void setName(std::string);
		std::string getName();
        void setUserCount(int userCount) { this->_userCount = userCount; }
        int getUserCount() { return this->_userCount;}
        user *getChannelMembers() { return this->_channelMembers;}
        void addUser(user newUser);
        void removeUser(user oldUser);
        //void createChannel(std::string channelName, int channelCount);

	private:
        std::string _channelName;
        user        *_channelMembers;
        int         _userCount;


};

#endif