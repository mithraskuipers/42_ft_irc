/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 08:52:59 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:32:43 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

/*
TODOS:
1. In main() add input checks.
1.1 Both provided?
1.2 Port only numerical?
1.3 Password not empty string e.g. ""
2. Controleren of we wel allowed functions gebruiken
3. Controleren of code memory leak vrij is
4. Commands toevoegen
4.1 USER-chan
QUIT
NOTICE
4.2 OPERATOR-san
KICK
INVITE
TOPIC
MODE
i: set/remove invite-only channel
t: set/remove restriction TOPIC operators
k: set/remove channel key
o: set/remove operator
l: set/remove user limit to channel

5. Moeten users in meerdere channels tegelijk kunnen zitten 💀 ? JA

IRSSI USAGE:
1. Terminal 1: ./ircserver 6667 pw
2. Terminal 2: irssi
3. In irssi doe je: /connect 127.0.0.1 6667 pw bas

IRSSI: SWITCH CHATS/CHANNELS
/window <window nummer hier>
*/

#include "./../incs/Server.hpp"

int main(int argc, char **argv)
{
	try
	{
		if (argc != 3)
		{
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		}

		Server	server(argv[1], argv[2]);
		
		server.startServer();
		return (0);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
