#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "includes.hpp"
#include <netinet/in.h> // struct sockaddr_in

class User;


class Channel
{
	public:
		Channel(); // default constructor
		Channel(std::string channelName);
		~Channel(); // destructor

		Channel &operator=(const Channel &src);
		void setName(std::string);
		std::string getName();
		void setUserCount(int userCount) { this->_userCount = userCount; }
		int getUserCount() { return this->_userCount;}
		User *getChannelMembers() { return this->_channelMembers;}
		void addUser(User newUser);
		void removeUser(User oldUser);
		//void createChannel(std::string channelName, int channelCount);

	private:
		std::string		_channelName;
		User			*_channelMembers;
		int				_userCount;
};

#endif