/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/18 22:50:12 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/19 21:00:25 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include "User.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

class Command {
public:
	Command();
	~Command();

	void commandHandler(User& Client);

private:
	std::unordered_map<std::string, std::function<void(const std::vector<std::string>&, User&)>> commandDictionary;
	std::unordered_map<std::string, std::vector<User>> channels;

	void createCommand(const std::vector<std::string>& cmds, User& Client);
	void joinCommand(const std::vector<std::string>& cmds, User& Client);
	void listCommand(const std::vector<std::string>& cmds, User& Client);
	void sendMessage(User& Client, const std::string& msgCode, const std::string& target, const std::string& message);
	void trim(std::string& str); // Declaration of trim function
	std::vector<std::string> splitCmd(const std::string& input);
};
