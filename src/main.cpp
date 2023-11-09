/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 08:52:59 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/08 20:49:23 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

/*
TODO ADD INPUT CHECKS
Both provided?
Port only numerical?
Password not empty string e.g. ""
*/

/*
Controleren of we wel allowed functions gebruiken
*/

/*
TODO CODE ZO LEK ALS EEN MANDJE?
*/

/*
COMMANDS:
MODE, KICK, QUIT, NOTICE
*/

/*
Hoe verbinden via irssi?
Compile je code
Run ./ircserver 6667 pw
Start irssi
In irssi doe je:
/connect 127.0.0.1 6667 pw bas
*/

/*
Hoe wissel je in irssi naar verschillende schermen?
Bijv. je zit in meerdere channels, en ontvangt op achtergrond ook prive berichten van anderen
In dat geval maakt irssi automatisch "windows" voor je aan. Iedere window heeft zijn eigen functie.
Bijv. een window gaat over channel A, en een andere window gaat over channel B, en een andere window betreft een prive gesprek tussen jou en een ander.
Hoe wissel je tussen die windows?
/window <window nummer hier>
Begin gewoon met 1 en probeer dan 2, 3 etc.
*/

#include "./../incs/Server.hpp"
#include "./../incs/splash.hpp"

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
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		return (1);
	}
}
