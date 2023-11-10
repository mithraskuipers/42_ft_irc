/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/10 08:17:07 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:17:08 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/utils.hpp"

void serverStdout(const std::string &firstMessageCombined)
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
	std::string str(buffer);
	std::cout << "[" << str << "] " << firstMessageCombined << std::endl;
};
