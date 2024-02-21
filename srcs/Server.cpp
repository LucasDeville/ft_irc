/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:50:58 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/21 14:08:22 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {
	
}

Server::Server(int port, std::string passwd) : _port(port), _passwd(passwd) {

	// new_channel( DEFAULT_CHANNEL );
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
	// _channel["*"]->addClient(*newClient);
	std::cout << "New client : " << csock << std::endl;
}

void Server::handleInput(unsigned long int i) {
	char buffer[8192];
	int err = recv(_client[i]->getSocket(), &buffer, sizeof(buffer), 0);
	if (err < 0)
		throw recvFailed();
	buffer[err] = '\0';
	if (err == 0)
	 	clientDisconnected(i);
	std::string str(buffer, strlen(buffer) - 1);
	parseBuffer(str, i);
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
	{
		_channel[name]->addClient(client);
		client.setChannel(_channel[name]);
	}
}

int	Server::sendAllClients(Channel *channel, int senderFd, std::string num, std::string message)
{
	std::vector<Client> clients = channel->getAllClients();
	std::vector<Client>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if (senderFd != it->getSocket())
			it->sendClient(num, message);
		it++;
	}
	return (1);
}

void	Server::parseBuffer(std::string buffer, int client) {

	std::cout << buffer << std::endl;
	std::string	commands[4] = {"PASS", "NICK", "USER", "JOIN"}; //CHANGE number for below 
	int	(Server::*_cPtr[4])(Parse parse, int client) = {&Server::cmdPass, &Server::cmdNick, &Server::cmdUser, &Server::cmdJoin};

	for(int i = 0; buffer[i]; i++) {
		if (buffer[i] == '\n')
			buffer[i] = 0; 
	}

	Parse parse(buffer);
	
	for (int i = 0; i < 4; i++) {
		if (parse.cmd.compare(commands[i]) == 0) {

			std::cout << "CMD : " << parse.cmd << std::endl;
			for (long unsigned int i = 0; i < parse.args.size(); i++)
				std::cout << parse.args[i] << std::endl;

			if (!(this->*_cPtr[i])(parse, client))
				std::cout << "Error command : " << commands[i] << std::endl;
		}
	}
}

int	Server::cmdPass(Parse parse, int c) {

	if (_client[c]->getAuth())
		return (_client[c]->sendClient("998", "You are already authentifiate."), 0);
	if (_client[c]->getRegistered()) {
		_client[c]->sendClient("462", "You are already registered");
		return (0);
	}

	//Paramètre vide - 461

	if (!strcmp(parse.args[0].c_str(), _passwd.c_str())) {
		_client[c]->setPass(parse.args[0]);
		_client[c]->setAuth(true);
		return (1);
	}
	_client[c]->sendClient("997", "Wrong password !");
	return (0);
}

int	Server::cmdNick(Parse parse, int c) {

	if (!_client[c]->getAuth())
		return (_client[c]->sendClient("998", "You need to authentificate with the PASS."), 0);
	if (parse.args[0].empty())
		return (_client[c]->sendClient("431", "Empty nickname !"), 0);

	for (int i = 0; parse.args[0][i]; i++) {
		if (!isalnum(parse.args[0][i]) && parse.args[0][i] != '-' && parse.args[0][i] != '\r')
			return (_client[c]->sendClient("432", "The nickname can't contain special character !"), 0);
	}

	//Nickname already use - 433

	_client[c]->setNickname(parse.args[0]);
	if (!_client[c]->getRegistered())
		_client[c]->notRegistered();
	return (1);
}

int	Server::cmdUser(Parse parse, int c) {

	if (!_client[c]->getAuth())
		return (_client[c]->sendClient("998", "You need to authentificate with the PASS."), 0);
	if (_client[c]->getRegistered()) 
		return (_client[c]->sendClient("462", "You are already registered"), 0);
	if (parse.args.size() < 4)
		return (_client[c]->sendClient("461", "Not enought parameter !"), 0);

	std::cout << "Username:|" << parse.args[0] << "|" << std::endl;
	_client[c]->setUser(parse.args[0]);
	
	std::cout << "Realname:|" << parse.args[3] << "|" << std::endl;
	_client[c]->setRealname(parse.args[3]);
	_client[c]->notRegistered();
	return (1);
}

int	Server::cmdJoin(Parse parse, int c) {

	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);

	// Pas assez de paramètre (1) - 461

	//Si arg = "0" - quitter tous les channels

	if (_channel.find(parse.args[0]) == _channel.end())
		new_channel(*_client[c], parse.args[0]);
	else
		join_channel(*_client[c], parse.args[0]);

	return (1);
}

int Server::cmdLeave(Parse parse, int c) {

//same as QUIT ? Leave = quit ?
	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);
	if (_client[c]->getChannel() == NULL)
	{
		_client[c]->sendClient("464", "Not in any channel.");
		return 0;	
	}
	
	(void) parse;
	std::map<std::string, Channel *>::iterator it;

	for (it = _channel.begin(); it != _channel.end(); ++it)
	{
    	if (it->second == _client[c]->getChannel())
		{
			_client[c]->setChannel(NULL);
			_client[c]->setMode(0);
			_channel.erase(it->first);
			return 1;
		}
	}
	return 0;
}

int Server::cmdQuit(Parse parse, int c) {
	(void) parse;
	clientDisconnected(c);
	return 1;
}

int Server::cmdOper(Parse parse, int c) {

	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);
	if (_client[c]->getChannel() == NULL)
	{
		_client[c]->sendClient("464", "Not in any channel."); //revoir code
		return 0;	
	}
	if (_client[c]->getMode())
	{
		_client[c]->sendClient("464", "Already an operator of this channel."); //revoir code
		return 0;
	}

	std::map<std::string, Channel *>::iterator it;
	(void) parse;
	
	//droit opérateur personnel est non sur chaque channel ?
	//Need un password transmis via str ?

	for (it = _channel.begin(); it != _channel.end(); ++it)
	{
    	if (it->second == _client[c]->getChannel())
		{
			if (!it->second->getOperatorList().empty())
			{
				_client[c]->sendClient("464", "Denied. If you want operator rights, ask an operator."); //revoir code
				return 0;
			}
			it->second->getOperatorList().push_back(*_client[c]);
			_client[c]->sendClient("381", "You have now operator rights !");
			_client[c]->setMode(1);
		}
	}
	
	return 1;
}

int Server::cmdTopic(Parse parse, int c) {
	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);
	if (_client[c]->getChannel() == NULL)
	{
		_client[c]->sendClient("442", "Not in any channel.");
		return 0;	
	}
	if (!_client[c]->getMode())
	{
		_client[c]->sendClient("482", "You don't have the rights for this.");
		return 0;
	}

	std::map<std::string, Channel *>::iterator it;
	
	for (it = _channel.begin(); it != _channel.end(); it++)
	{
    	if (it->second == _client[c]->getChannel())
		{
			if (parse.args[0].empty())
				_client[c]->sendClient("332", it->second->getTopic()); //verif si topic vide ? Si oui 331
			else
				it->second->addTopic(parse.args[0]);
		}
	}
	
	return 1;
}

int Server::cmdKick(Parse parse, int c) {
	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);
	if (_client[c]->getChannel() == NULL)
	{
		_client[c]->sendClient("442", "Not in any channel.");
		return 0;	
	}
	if (!_client[c]->getMode())
	{
		_client[c]->sendClient("482", "You don't have the rights for this.");
		return 0;
	}
	
	for (unsigned int i = 0; i < _client.size(); i++)
	{
		if (_client[i]->getNickname() == parse.args[0])
		{
			// _client[i]->sendClient("You have been kicked by " + _client[c]->getNickname());
			// _client[c]->sendClient("Successfully kicked " + _client[i]->getNickname());

			//No message ? except "KICK channelName"

			// send channel "_client[i] has been kicked by _client[c]"
			clientDisconnected(i);
			return 1;
		}
	}
	
	_client[c]->sendClient("441", "Wrong username or not in that channel.");

	//Channel doest not exist ? - 403
	
	return 0;
}

int Server::cmdPM(Parse parse, int c) {
	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);
	
	(void) parse;

	//On peut envoyer des messages prive pas juste channel
	/*if (_client[c]->getChannel() == NULL)
	{
		_client[c]->sendClient("442", "Not in any channel.");
		return 0;	
	}

	if (!_client[c]->getMode())
	{
		_client[c]->sendClient("482", "You don't have the rights for this.");
		return 0;
	}
	
	for (unsigned int i = 0; i < _client.size(); i++)
	{
		if (_client[i]->getNickname() == str)
		{
			_client[i]->sendClient("You have been kicked by " + _client[c]->getNickname());
			_client[c]->sendClient("Successfully kicked " + _client[i]->getNickname());
			// send channel "_client[i] has been kicked by _client[c]"
			clientDisconnected(i);
			return 1;
		}
	}
	
	_client[c]->sendClient("Wrong username.");*/
	return 0;
}

