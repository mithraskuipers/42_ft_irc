/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   includes.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/07/08 14:12:24 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/24 22:28:58 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_HPP

#include <iostream> // cout
#include <cstdlib>	// atoi
#include "Server.hpp"
#include <cstdio> // printf
#include "splash.hpp"
#include <unistd.h>
#include <algorithm>
#include "Client.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include <cstring> // string.h
#include <algorithm>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include "./../incs/Server.hpp"
#include <cstdlib>
#include <ctime>
#include "./../incs/splash.hpp"

#define QUEUE_SIZE 10
#define TRUE 1
#define FALSE 0

#define SERVER_NAME "chat.debugdivas.nl"

#endif