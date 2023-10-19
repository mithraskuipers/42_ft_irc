/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/18 22:50:12 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/19 12:33:49 by mikuiper      ########   odam.nl         */
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
		void commandHandler(User& client);

	private:
		std::vector<std::string> splitCmd(const std::string& input, const std::string& delimiter);
		void sendMessage(User& client, const std::string& msgCode, const std::string& target, const std::string& message);
		void joinCommand(const std::vector<std::string>& cmds, User& client);
		void createCommand(const std::vector<std::string>& cmds, User& client);
		void listCommand(const std::vector<std::string>& cmds, User& client);


		std::map<std::string, std::function<void(const std::vector<std::string>&, User&)>> commandDictionary;
};

#endif // COMMAND_HPP