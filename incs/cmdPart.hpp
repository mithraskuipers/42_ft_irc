/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cmdPart.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/10 08:16:47 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:16:48 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CMDPART_HPP
# define CMDPART_HPP

class cmdPart : public Command
{
public:
	cmdPart(Server *server, bool registrationRequired);
	~cmdPart();

	void execute(Client *client, std::vector<std::string> arguments);
};

#endif