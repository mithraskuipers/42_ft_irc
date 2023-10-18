/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   User.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/17 22:22:03 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/18 23:02:04 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */



#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class User
{
    public:
        User(int socket_descriptor, const std::string& nick);
        int getSocketDescriptor() const;
        std::string& getBuff();
        std::string getIP() const;
        void setIP(const std::string& clientIP);
        int getPort() const;
        void setPort(int clientPort);
        std::string getNick() const; // Added method declaration for getNick
		bool getRegisteredStatus() const;

    private:
        int socket_descriptor;
        std::string buff; // Buffer to store received data
        std::string ip;   // User's IP address
        int port;         // User's port
        std::string nickname; // User's nickname
		bool registered;
};

#endif
