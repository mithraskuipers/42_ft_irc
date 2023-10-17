/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mikuiper <mikuiper@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/10/17 22:22:03 by mikuiper      #+#    #+#                 */
/*   Updated: 2023/10/17 22:40:48 by mikuiper      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLIENT_HPP
#define CLIENT_HPP

class Client 
{
	public:
		Client(int socket_descriptor) : socket_descriptor(socket_descriptor) {}
		int getSocketDescriptor() const { return socket_descriptor; }
	private:
		int socket_descriptor;
};


#endif