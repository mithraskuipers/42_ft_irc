/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cmdJoin.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/10 08:16:43 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:16:44 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CMDJOIN_HPP
# define CMDJOIN_HPP

class cmdJoin : public Command
{
public:
	cmdJoin(Server *server, bool registrationRequired);
	~cmdJoin();

	void execute(Client *client, std::vector<std::string> arguments);
};

#endif