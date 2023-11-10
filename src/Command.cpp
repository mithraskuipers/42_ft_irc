/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/07 22:14:10 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:18:10 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/Command.hpp"

/*
********************************************************************************
ORTHODOX CANONICAL FORM
********************************************************************************
*/

Command::Command(Server *server, bool registrationRequired) : server(server), _registrationRequired(registrationRequired)
{

}

Command::~Command()
{

}

/*
********************************************************************************
MEMBER FUNCTIONS
********************************************************************************
*/

bool Command::registrationRequired() const
{
	return (_registrationRequired);
}
