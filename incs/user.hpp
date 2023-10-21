#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <list>
#include "channel.hpp"

class channel;

class user {
public:
    user(int socket_fd = 0);
    ~user();


    void setName(const std::string& username);
    void setPass(const std::string& password);
    std::string getName() const;
    std::string getPass() const;

    int getSocket() const;
    void setSocket(int socket_fd);

    int getID() const;
    void setID(int id);
    void commandHandler(const std::string& buffer, std::list<channel>& channelList);

private:
    int _socket;
    int _id;
    std::string _username;
    std::string _password;
};

#endif // USER_HPP
