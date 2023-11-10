/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   JoinCommand.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/10 08:16:52 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:16:53 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CMDJOIN_HPP
# define CMDJOINexecute_HPP

class cmdJoin : public Command
{
public:
	cmdJoin(Server *server);
	~cmdJoin();

	void execute(Client *client, std::vector<std::string> arguments);
};

#endif