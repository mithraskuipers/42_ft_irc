/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   includes.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/07/08 14:12:24 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/09/06 22:03:57 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#include <iostream>
#include <cstdlib> // atoi
#include "IRCServer.hpp"
#include "user.hpp"
#include "channel.hpp"
#include <cstdio> // printf
#include "splash.hpp"
#include <unistd.h>
#include <algorithm>
#include <cstring> // use <cstring> instead of "string.h"
# define TRUE 1
# define FALSE 0
# define ESC_KEY 27

#ifndef MAX_CLIENTS
#define MAX_CLIENTS 100
#endif

#ifndef QUEUE_SIZE
#define QUEUE_SIZE 10
#endif

#endif // INCLUDES_HPP
