/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:48:01 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/15 15:59:01 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "common.hpp"

class Server {

private:

	int									_serverSock;
	unsigned short						_port;
	std::string							_passwd;
	std::vector<pollfd>					_pollfd;
	std::vector<Client *>				_client;
	std::map<std::string, Channel *>	_channel;
	Server();

public:

	Server(int port, std::string passwd);
	~Server();

	void	createServer();
	void	serverLoop();
	void	acceptClient();
	void	handleInput(long unsigned int i);
	void	clientDisconnected(long unsigned int i);
	void 	new_channel(std::string const & name);
	void 	new_channel(Client const & client, std::string const & name);

	void	setPass(int i, std::string const & pass);
};

#endif