#ifndef USER_HPP
#define USER_HPP

#include <poll.h>
#include <string>
#include "./../incs/Channel.hpp"

class Client
{
	private:
		int socket_descriptor;
		int port;
		std::string _nickname;
		std::string buff;
		bool registered;
		std::string ip;
		std::string _realname;
		std::vector<Channel> channels;
		pollfd polledFD;
	
	public:
		void sendToClient(const std::string &data);
		Client(int socket_descriptor, const std::string &nick = "");
		int getSocketDescriptor() const;
		std::string &getBuff();
		void setNick(const std::string &newNick);
		void setRealName(const std::string &newRealName);
		std::string getRealName() const;
		void send(const std::string &data);
		std::string getIP() const;
		void setIP(const std::string &clientIP);
		int getPort() const;
		pollfd getPolledFD() const;
		void setPort(int clientPort);
		std::string getNick() const;
		bool getRegisteredStatus() const;
		void setRegistered(bool value);
		void addChannel(const Channel &channel) { channels.push_back(channel); }
};

#endif
