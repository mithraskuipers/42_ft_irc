/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/18 22:50:12 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/18 23:53:02 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "User.hpp"
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>

typedef void (*pfunc)(const std::vector<std::string>&, User&);

class Command
{
	public:
		Command();
		~Command();
		void						processCommand(User& client);

	private:
		std::vector<std::string>	splitCmd(const std::string& input, const std::string& delimiter);
		void 						sendMessage(User& client, const std::string& msgCode, const std::string& command, const std::string& message);
		std::map<std::string, void (Command::*)(const std::vector<std::string>&, User&)>	commandDictionary; // DEZE DICTIONARY MOET NOG POPULATED WORDEN MET FUNCTIES ZOALS /NICK EN /JOIN
};

#endif // COMMAND_HPP