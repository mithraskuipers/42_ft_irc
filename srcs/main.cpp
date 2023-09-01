/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/07/08 14:09:49 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/08/30 17:39:29 by mkuipers      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/includes.hpp"
#include "./../incs/IRCServer.hpp"

int err_msg_and_return(std::string s, int ret_val)
{
	std::cout << s << std::endl;
	return (ret_val);
}

int	main(int argc, char **argv)
{
	if (argc != 3)
		return (err_msg_and_return("Error: Usage: ./ircserv <port #> <password>", 1));
	int port = atoi(argv[1]);

	// Ports below 1024 are privileged
	if (port < 1024 || port > 65535)
		return (err_msg_and_return("Error: <port #> must be an int >= 1024 and =< 65535", 1));
	// Here start the server
	IRCServer server(port, argv[2]);
	try
	{
		server.initServer();
		server.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}


/*
Remarks:
- Laat voor nu maar de {} staan, zelfs als het niet nodig is voor single line code blocks.
Dit maakt het makkelijk voor ons om extra print statements toe the voegen en debuggen.
Maar ook het toevoegen van comments boven de line.
*/

/*
Current problems:
- Parameters port and password not correctly shown in output
*/

/*
Must know:

- socket is combination of ip and port
e.g. 192.168.1.10:8080
socket will be used to listen for incoming connections from clients

- binding
after creating a socket, you need to bind the socket to a specific ip socket_address and port

*/