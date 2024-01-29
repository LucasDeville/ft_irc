/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:50:58 by ldeville          #+#    #+#             */
/*   Updated: 2024/01/29 10:33:19 by bpleutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {
	
}

Server::Server(int port, std::string passwd) : _port(port), _passwd(passwd) {

	new_channel( DEFAULT_CHANNEL );
}

Server::~Server() {
	
}

void Server::createServer() {
	sockaddr_in saddr;
	pollfd		poll;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0)
		throw socketFailed();
	memset(&saddr, 0, sizeof(sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(this->_port);
	saddr.sin_addr.s_addr = *((unsigned long *)gethostbyname("localhost")->h_addr_list[0]);
	if (bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
		throw bindingFailed();
	if (listen(sockfd, 5) == -1)
		throw listenFailed();

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) //non blocking
		throw listenFailed();

	memset(&poll, 0, sizeof(poll));
	poll.fd = sockfd;
	poll.events = POLLIN;
	_serverSock = sockfd;
	_pollfd.push_back(poll);
	_client.push_back(new Client(_serverSock));
	serverLoop();
}

void Server::serverLoop() {
	while (1)
	{
		if (poll(_pollfd.data(), _pollfd.size(), -1) == -1)
			throw pollFailed();
		for (long unsigned int i = 0; i < _pollfd.size(); i++)
		{
			if (_pollfd[i].revents & POLLHUP)
			{
				clientDisconnected(i);
				break;
			}
			if (_pollfd[i].revents & POLLIN)
			{
				if (_pollfd[i].fd == _serverSock)
					acceptClient();
				else
					handleInput(i);
			}
		}
	}
}

void Server::clientDisconnected(long unsigned int i){
	std::cout << "IRC: Client '" << _client[i]->getNickname() << "' connection closed." << std::endl;
	close(_pollfd[i].fd);
	_pollfd.erase(_pollfd.begin() + i);
}

void Server::acceptClient() {
	int csock;
	sockaddr_in caddr;
	pollfd pollFd;
	
	socklen_t clen = sizeof(caddr);
	if ((csock = accept(_serverSock, (struct sockaddr *) &caddr, &clen)) < 0)
		throw acceptFailed();

	// Set the client socket to non-blocking
	int flags = fcntl( pollFd.fd, F_GETFL, 0 );
	fcntl( pollFd.fd, F_SETFL, flags | O_NONBLOCK );

	pollFd.fd = csock;
	pollFd.events = POLLIN | POLLOUT;
	pollFd.revents = 0;

	/* create new Client */
	_pollfd.push_back(pollFd);
	Client *newClient = new Client(csock);
	_client.push_back(newClient);
	_channel["*"]->addClient(*newClient);
	std::cout << "New client : " << csock << std::endl;
}

void Server::handleInput(unsigned long int i) {
	char buffer[2048];
	int err = recv(_client[i]->getSocket(), &buffer, sizeof(buffer), 0);
	if (err < 0)
		throw recvFailed();
	buffer[err] = '\0';
	if (err == 0)
	 	clientDisconnected(i);
	parseBuffer(buffer, i);
	// std::cout << _client[i]->getNickname() << " (" << i << "):" << buffer << std::endl;
}

void Server::new_channel(std::string const & name)
{
	Channel * channel = new Channel(name);
	_channel.insert( std::pair<std::string, Channel *>( name, channel ) );
}

void Server::new_channel(Client & client, std::string const & name)
{
	Channel * channel = new Channel(name, client);
	_channel.insert( std::pair<std::string, Channel *>( name, channel ) );
}

void Server::join_channel(Client & client, std::string const & name)
{
	if (_channel.find(name) != _channel.end())
		_channel[name]->addClient(client);
}

void	Server::setPass(int i, std::string const & pass) { _client[i]->setPass(pass); };


void	Server::parseBuffer(char buffer[2048], int client) {

	std::cout << buffer << std::endl;
	std::string	commands[4] = {"PASS", "NICK", "USER", "JOIN"}; //CHANGE number for below 
	int	(Server::*_cPtr[4])(std::string str, int client) = {&Server::cmdPass, &Server::cmdNick, &Server::cmdUser, &Server::cmdJoin};



	for(int i = 0; buffer[i]; i++) {
		if (buffer[i] == '\n')
			buffer[i] = 0; 
	}
	std::cout << buffer << std::endl;

	for (int i = 0; i < 4; i++) {
		if (!strncmp(buffer, commands[i].c_str(), commands[i].size())) {
			if ((this->*_cPtr[i])(&buffer[commands[i].size() + 1], client))
				std::cout << "Error command : " << commands[i] << std::endl;
		}
	}
}

int	Server::cmdPass(std::string pass, int c) {

	if (_client[c]->getRegistered()) {
		_client[c]->sendClient("Already registered");
		return (0);
	}

	if (pass[0] == ':')
		pass = &pass[1];
		
	std::cout << "|" << pass << "|" << _passwd << std::endl;
	
	if (!strcmp(pass.c_str(), _passwd.c_str())) {
		_client[c]->setPass(pass);
		if (!_client[c]->getRegistered())
			_client[c]->notRegistered();
		return (1);
	}
	_client[c]->sendClient("Wrong password");
	return (0);
}

int	Server::cmdNick(std::string nick, int c) {

	if (nick.empty()) {
		_client[c]->sendClient("Empty nickname");
		return (0);
	}
	_client[c]->setNickname(nick);
	if (!_client[c]->getRegistered())
		_client[c]->notRegistered();
	return (1);
}

int	Server::cmdUser(std::string str, int c) {

	if (_client[c]->getRegistered()) {
		_client[c]->sendClient("Already registered");
		return (0);
	}
	
	int i = 0;
	while (str[i] && str[i] == ' ')
		i++;
	int len = i;
	while (str[len] && str[len] != ' ')
		len++;

	std::cout << "Username:|" << str.substr(i, len) << "|" << std::endl;
	_client[c]->setUser(str.substr(i, len));

	while (str[++i] && str[i] != ':');
	i++;
	len = i;
	while (str[len] && str[len] != '\n')
		len++;


	std::cout << "Realname:|" << str.substr(i, len) << "|" << std::endl;
	_client[c]->setRealname(str.substr(i, len));
	_client[c]->notRegistered();
	return (1);
}

int	Server::cmdJoin(std::string str, int c) {

	if (!_client[c]->getRegistered())
		return 0;

	if (_channel.find(str) == _channel.end())
		new_channel(*_client[c], str);
	else
		join_channel(*_client[c], str);


	return (1);
}