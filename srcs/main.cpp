/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/07/08 14:09:49 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/20 11:34:22 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/includes.hpp"
#include "./../incs/splash.hpp"

int err_msg_and_return(std::string s, int ret_val)
{
	std::cout << s << std::endl;
	return (ret_val);
}

int	main(int argc, char **argv)
{
	splash();
	if (argc != 3)
		return (err_msg_and_return("Error: Usage: ./ircserv <port #> <password>", 1));
	int port = atoi(argv[1]);

	if (port < 1024 || port > 65535)											// Ports below 1024 are reserved

		return (err_msg_and_return("Error: <port #> must be an int >= 1024 and =< 65535", 1));

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
Server Socket:
This is the socket used by the server program to listen for incoming connections from clients.
It's also known as the listening socket. The server socket is passive and waits for clients to connect to it.

User Socket:
This is the socket used by the User program to initiate a connection to the server.
It's an active socket that initiates the communication by connecting to the server's listening socket.
*/

/*
After connecting with an IRC server, you typically do the following:
1. /nick <CHOSEN NICKNAME>														<-- TODO: What if the nickname is already registered?
2. /join <CHOSEN CHANNEL INSIDE THE SERVER>										<-- Works after having chosen a nickname.
3. <YOUR MESSAGE HERE>															<-- Without any specific command this will broadcast a message to everyone in the Channel.
3. /msg <SOMEONE'S NICKNAME> <YOUR MESSAGE HERE>								<-- Send a private message to another User.
*/

/*
[socket]
A combination of ip and port
e.g. 192.168.1.10:8080
socket will be used to listen for incoming connections from clients

[binding]
after creating a socket, you need to bind the socket to a specific ip socket_address and port
*/

/*
Classes overview:

[User]
Heeft info. over individuele clients, zoals sockets, IP, ports, nickname, password, etc.

[Channel]
Heeft info. over clients die in de channel zitten.
Clients kunnen worden toegevoegd/verwijderd via een member function van Channel.
TODO: Uitnodigen clients naar channels.
TODO: Verander de channel topic. Dit wordt gedaan door client die verbonden is met die channel.
TODO: Kick a client. Dit wordt gestart door een andere client.

[Command]
Deze class bevat alle server commando's (e.g. JOIN).
De class voegt ze ook samen in een dictionary.

[Server]
Initialiseert de server sockets, options, en koppelt de server aan een specifieke poort.
Low-level networking aspecten van de server.

Accepteert verbindingen van clients via accept().
Nieuwe clients worden opgeslagen in de Client objects.

Parsed en execute ook commands van de clients.
*/