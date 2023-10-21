/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   IRCServer.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: mkuipers <mkuipers@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/09/01 17:56:34 by mkuipers      #+#    #+#                 */
/*   Updated: 2023/10/22 01:24:37 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include "includes.hpp"
#include <netinet/in.h> // struct sockaddr_in
#include <list>
#include "Command.hpp"
#include "Channel.hpp" // Include the Channel.hpp header file
#include "User.hpp"    // Include the User.hpp header file

class IRCServer
{

private:
    unsigned int active_users;
    int port;
    std::string password;          // Declare password member before clients member
    std::vector<User> clients;    // Add a vector to store clients
    Command command;
    int server_listening_socket;
    std::string IP;
    struct sockaddr_in socket_address;
    std::vector<Channel> channels; // Declare the channels vector here
    std::string welcomeMessage;    // Add this member variable

public:
	std::string getPass();
    IRCServer(int port, const std::string &password); // Constructor takes port and password as arguments
    ~IRCServer();
    void initServer();                      // initialize server
    void start();                           // start server
    void getHostIP();                       // get host ip
    std::string getIP() const;
    int updateMaxSocketDescriptor();
    int getPort() const;
    std::string addClientSocket(int clientSocket); // Update the declaration
    std::string generateRandomCode();
    bool isNicknameInUse(const std::string &nickname) const;
};

#endif
