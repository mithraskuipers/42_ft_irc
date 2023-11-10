/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cmdPass.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/10 08:16:49 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/11/10 08:16:50 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CMDPASS_HPP
#define CMDPASS_HPP

class cmdPass : public Command
{
	public:
		cmdPass(Server *server, bool registrationRequired);
		~cmdPass();

		void execute(Client *client, std::vector<std::string> arguments);
};

#endif