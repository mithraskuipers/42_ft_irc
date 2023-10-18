/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_split.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/18 23:43:16 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/18 23:43:52 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "./../incs/includes.hpp"

std::vector<std::string> ft_split(const std::string& input, const std::string& delimiter)
{
	std::vector<std::string> tokens;
	std::size_t start = 0;
	std::size_t end = input.find(delimiter);

	while (end != std::string::npos)
	{
		tokens.push_back(input.substr(start, end - start));
		start = end + delimiter.length();
		end = input.find(delimiter, start);
	}

	tokens.push_back(input.substr(start));

	return tokens;
}