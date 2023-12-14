/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:48:01 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/14 12:54:40 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "common.hpp"

class Server {

private:

	unsigned short			_port;
	std::string				_passwd;
	std::vector<pollfd>		_pollfd;
	std::vector<Client *>	_client;
	Server();

public:

	Server(int port, std::string passwd);
	~Server();
	void createServer();
	void serverLoop(int sockfd);
	void acceptClient(int sockfd);
};

#endif