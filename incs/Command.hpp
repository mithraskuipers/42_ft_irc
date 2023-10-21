#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>

class IRCServer; // Forward declaration of IRCServer class
class Channel;   // Forward declaration of Channel class
class User;

class Command
{
private:
    std::vector<User>& clients;
    IRCServer& ircServer; // Declare ircServer as a member variable
public:
    ~Command();
    Command(std::vector<User> &clients, IRCServer &server);
    void process(const std::string &input, User &user, std::vector<Channel> &channels);
    void sendChannelList(User &user, const std::vector<Channel> &channels);
    void sendNumericReply(User &user, int numericCode, const std::string &message);
};

#endif // COMMAND_HPP
