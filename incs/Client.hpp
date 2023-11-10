/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/10 08:16:41 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:16:42 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP



class Client;

#include <vector>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include "./../incs/Channel.hpp"

class Client
{

	typedef std::vector<pollfd>::iterator pollfds_iterator;

private:
	int _fd;
	std::string _hostname;
	int port;

	std::string clientNickname;
	std::string clientUsername;
	std::string clientRealname;

	int clientRegistrationLevel;

	Channel *_channel;

public:
	Client(int fd, const std::string &hostname, int port);
	~Client();
	int getFD() const;
	std::string getHostname() const;
	int getPort() const;
	bool isRegistered() const;
	std::string getNickname() const;
	std::string getUsername() const;
	std::string getRealName() const;
	bool isInChannel(const std::string &channelName) const;
	std::string getPrefix() const;
	Channel *getChannelInstance() const;
	void setNickname(const std::string &nickname);
	void setUsername(const std::string &username);
	void setRealName(const std::string &realname);
	void setRegistrationLevel(int updatedRegistrationLevel);
	void setChannel(Channel *channel);
	void sendMessageToClientItself(const std::string &firstMessageCombined) const;
	void welcomeMessage();
	void clientJoinChannel(Channel *channel);
	void clientLeaveChannel();
};

#endif
