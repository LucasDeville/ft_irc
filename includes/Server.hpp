/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:48:01 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/20 16:12:32 by bpleutin         ###   ########.fr       */
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

	void	setPass(int i, std::string const & pass);


	void	parseBuffer(char buffer[2048], int i);

	//commands
	int		cmdPass(std::string pass, int c);
	int		cmdNick(std::string nick, int c);
	int		cmdUser(std::string str, int c);
	int		cmdJoin(std::string str, int c);
	
	//done but not tested
	int		cmdLeave(std::string str, int c);
	int		cmdQuit(std::string str, int c);
	int		cmdOper(std::string str, int c);
	int		cmdTopic(std::string str, int c);
	int		cmdKick(std::string str, int c);
	//to-do
	int		cmdInv(std::string str, int c);
	int		cmdMode(std::string str, int c); // examples: MODE +i, MODE -i... MODE alone displays channel's modes. (MODE i displays i ?)
	int		cmdPM(std::string str, int c);
	int		cmdSendF(std::string str, int c);
	int		cmdGetF(std::string str, int c);
	int		cmdBot(std::string str, int c);
};

#endif