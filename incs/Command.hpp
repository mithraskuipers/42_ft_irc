// Command.hpp

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>

class User;
class Channel;

class Command
{

private:
    std::vector<User>& clients; // Add a vector to store clients
public:
	~Command();
	Command(std::vector<User>& clients);
	void process(const std::string &input, User &user, std::vector<Channel> &channels);
	void sendChannelList(User &user, const std::vector<Channel> &channels);
	void sendNumericReply(User &user, int numericCode, const std::string &message);
	
};

#endif // COMMAND_HPP
