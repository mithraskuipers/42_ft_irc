/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   User.hpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/19 13:52:04 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/20 09:19:36 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include <string>

class User {
public:
	User(int socket_descriptor, const std::string& nick);
	int getSocketDescriptor() const;
	std::string& getBuff();
	std::string getIP() const;
	void setIP(const std::string& clientIP);
	int getPort() const;
	void setPort(int clientPort);
	std::string getNick() const;
	bool getRegisteredStatus() const;
	void setPass(const std::string& password);
	void setName(const std::string& username);
	int getID() const;

private:
	int socket_descriptor;
	std::string buff;
	std::string ip;
	int port;
	std::string _nickname;
	bool registered;
	std::string _password;
	int _id;
};

#endif
