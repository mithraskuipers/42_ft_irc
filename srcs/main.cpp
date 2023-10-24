/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/07/08 14:09:49 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/24 19:30:41 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


/*

[KVIRC]
/LIST
/join kip -> maakt #kip


irssi settings
/set rejoin_channels_on_reconnect off
/set channels_rejoin_unavailable off

*/

#include "./../incs/includes.hpp"

int err_msg_and_return(std::string s, int ret_val)
{
	std::cout << s << std::endl;
	return (ret_val);
}

int main(int argc, char **argv)
{
	int port;
	std::string password;
	splash();
	if (argc != 3)
		return (err_msg_and_return("Error: Usage: ./ircserv <port #> <password>", 1));
	port = atoi(argv[1]);
	password = argv[2];

	if (port < 1024 || port > 65535) // Ports below 1024 are reserved

		return (err_msg_and_return("Error: <port #> must be an int >= 1024 and =< 65535", 1));

	Server server(port, password);
	try
	{
		server.initServer();
		server.startServer();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: Exception caught - " << e.what() << std::endl;
	}
	return (0);
}
