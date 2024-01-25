/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:48:01 by ldeville          #+#    #+#             */
/*   Updated: 2024/01/25 15:52:16 by ldeville         ###   ########.fr       */
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


	void	parseBuffer(char buffer[2048], int i);

	//commands
	int		cmdPass(std::string pass, int c);
	int		cmdNick(std::string nick, int c);
	int		cmdUser(std::string str, int c);
};

#endif