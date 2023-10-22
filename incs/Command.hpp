// Command.hpp
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>

class IRCServer; // Forward declaration of IRCServer class
class User;
class Channel; // Forward declaration of Channel class

class Command
{
private:
    std::vector<User> &clients;
    IRCServer &ircServer;
    std::vector<Channel> channels; // Store channels directly in the Command class

public:
    ~Command();
    Command(std::vector<User> &clients, IRCServer &server);

    void process(const std::string &input, User &user);
    void sendChannelList(User &user);
    void sendNumericReply(User &user, int numericCode, const std::string &message);

    // Add methods for channel management within the Command class
    void addChannel(const std::string &channelName);
    bool joinChannel(const std::string &channelName, User &user);
    bool leaveChannel(const std::string &channelName, User &user);
};

#endif // COMMAND_HPP
