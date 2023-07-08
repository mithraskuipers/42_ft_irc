/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/07/08 14:09:49 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/07/08 14:23:09 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/includes.hpp"

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
	if (port < 1024 || port > 65535)
		return (err_msg_and_return("Error: <port #> must be an int >= 1024 and =< 65535", 1));
	return (0);
}