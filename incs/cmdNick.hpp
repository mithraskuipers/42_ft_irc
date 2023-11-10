/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cmdNick.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/10 08:16:45 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:16:47 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CMDNICK_HPP
# define CMDNICK_HPP

class cmdNick : public Command
{
public:
	cmdNick(Server *server, bool registrationRequired);
	~cmdNick();

	void execute(Client *client, std::vector<std::string> arguments);
};


#endif 