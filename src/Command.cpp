/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/07 22:14:10 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/07 22:25:13 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Command.hpp"

Command::Command(Server *server, bool registrationRequired) : server(server), _registrationRequired(registrationRequired)
{

}

Command::~Command()
{

}

bool Command::registrationRequired() const
{
	return _registrationRequired;
}
