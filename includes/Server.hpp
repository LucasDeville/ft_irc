/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:48:01 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/14 14:04:04 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "common.hpp"

class Client;

class Server {

private:

	int						_serverSock;
	unsigned short			_port;
	std::string				_passwd;
	std::vector<pollfd>		_pollfd;
	std::vector<Client *>	_client;
	Server();

public:

	Server(int port, std::string passwd);
	~Server();

	void	createServer();
	void	serverLoop();
	void	acceptClient();
	void	handleInput(int i);
	// void	quitServer(int i);
	// void	clientDisconnected(int i);
};

#endif