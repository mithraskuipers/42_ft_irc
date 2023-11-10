/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/07 22:14:35 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:11:52 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>
#include "./../incs/Client.hpp"
#include "./../incs/Server.hpp"

class Server;

class Command
{
	protected:
		Server *server;
		bool _registrationRequired;												// Bool indicating if authentication is required for the command

	public:
		explicit Command(Server *server, bool registrationRequired);			// Constructor with optional registrationRequired parameter
		virtual ~Command();														// Virtual destructor for proper cleanup
		bool registrationRequired() const;										// Member function to check if authentication is required
		virtual void execute(Client *client, std::vector<std::string> arguments) = 0;	// Pure virtual function to be implemented by derived classes
};

#endif // End of the header guard
