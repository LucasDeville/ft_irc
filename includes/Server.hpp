/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:48:01 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/21 11:33:59 by ldeville         ###   ########.fr       */
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
	void 	new_channel(Client & client, std::string const & name);
	void 	join_channel(Client & client, std::string const & name);

	int		sendAllClients(Channel *channel, int senderFd, std::string num, std::string message);

	void	parseBuffer(std::string buffer, int i);

	//commands
	int		cmdPass(Parse parse, int c);
	int		cmdNick(Parse parse, int c);
	int		cmdUser(Parse parse, int c);
	int		cmdJoin(Parse parse, int c);
	
	//done but not tested
	int		cmdLeave(Parse parse, int c);
	int		cmdQuit(Parse parse, int c);
	int		cmdOper(Parse parse, int c);
	int		cmdTopic(Parse parse, int c);
	int		cmdKick(Parse parse, int c);
	int		cmdPM(Parse parse, int c);
	//to-do
	int		cmdInv(Parse parse, int c);
	int		cmdMode(Parse parse, int c); // examples: MODE +i, MODE -i... MODE alone displays channel's modes. (MODE i displays i ?)
	int		cmdSendF(Parse parse, int c);
	int		cmdGetF(Parse parse, int c);
	int		cmdBot(Parse parse, int c);
};

#endif