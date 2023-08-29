/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   IRCServer.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: mkuipers <mkuipers@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/08/29 10:42:04 by mkuipers      #+#    #+#                 */
/*   Updated: 2023/08/29 11:50:42 by mkuipers      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/includes.hpp"
#include "./../incs/IRCServer.hpp"

#include <unistd.h> // gethostname()

IRCServer::IRCServer() : port(42), password("")
{
    getHostname();
}

IRCServer::IRCServer(int port, const std::string &password)
{
    (void)port;
    (void)password;
    getHostname();
}

void IRCServer::getHostname()
{
    char    host[256];
    struct hostent *host_entry;
    gethostname(host, sizeof(host));
    (void)host_entry;

}

void IRCServer::init()
{
    //
}

void IRCServer::start()
{
    //
}

IRCServer::~IRCServer()
{
    //
}