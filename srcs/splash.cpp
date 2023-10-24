/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   splash.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mkuipers <mkuipers@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/09/01 17:37:03 by mkuipers      #+#    #+#                 */
/*   Updated: 2023/10/24 22:32:11 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/includes.hpp"

void color_red()
{
	printf("\033[0;31m");
}

void color_orange()
{
	printf("\033[38;5;214m");
}

void color_green()
{
	printf("\033[0;32m");
}

void color_reset()
{
	printf("\033[0m");
}

void clear_screen(void)
{
	write(1, "\x1b[2J", 4);
	write(1, "\x1b[H", 3);
}

void splash()
{
	clear_screen();
	color_orange();
	printf("%s", SPLASH);
	color_reset();
}