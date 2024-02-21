/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:50:58 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/21 20:59:55 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int alive;

void handleCtrl(int sig) {
	if (sig == SIGINT)
		alive = 0;
}

Server::Server() {
	
}

Server::Server(int port, std::string passwd) : _port(port), _passwd(passwd) {

	alive = 1;
	// new_channel( DEFAULT_CHANNEL );
}

Server::~Server() {
	std::vector<Client *>::iterator it = _client.begin();
	while (it != _client.end()) {
		delete (*it);
		it++;
	}
}

void Server::createServer() {
	signal(SIGINT, handleCtrl);
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
	while (alive)
	{
		if (poll(_pollfd.data(), _pollfd.size(), -1) == -1)
			throw pollFailed();
		for (long unsigned int i = 0; i < _pollfd.size(); i++)
		{
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

void Server::clientDisconnected(long unsigned int i, int cli) {
	std::cout << "IRC: Client '" << _client[i]->getNickname() << "' connection closed." << std::endl;
	close(_client[cli]->getSocket());
	_pollfd.erase(_pollfd.begin() + i);
	if (_client[cli]->getChannel() != NULL)
		_client[cli]->getChannel()->deleteClient(*_client[cli], *this);
	delete _client[cli];
	_client.erase(_client.begin() + cli);
}

void Server::acceptClient() {
	int csock;
	sockaddr_in caddr;
	pollfd pollFd;

	socklen_t clen = sizeof(caddr);
	if ((csock = accept(_serverSock, (struct sockaddr *) &caddr, &clen)) < 0)
		throw acceptFailed();

	// Set the client socket to non-blocking
	int flags = fcntl( csock, F_GETFL, 0 );
	fcntl( csock, F_SETFL, flags | O_NONBLOCK );

	pollFd.fd = csock;
	pollFd.events = POLLIN;
	pollFd.revents = 0;

	_pollfd.push_back(pollFd);
	Client *newClient = new Client(csock);
	_client.push_back(newClient);

	newClient->sendWelcome();
	// _channel["*"]->addClient(*newClient);
	std::cout << "New client : " << csock << std::endl;
}

int Server::getClientIndex(int fd) {
	int index = 0;

	while (_client[index]) {
		if (_client[index]->getSocket() == fd)
			return index;
		index++;
	}

	std::cout << "getClientIndex : Error no client with this fd !" << std::endl;
	return -1;
}

void Server::handleInput(unsigned long int i) {
	char buffer[8192];
	int err = recv(_pollfd[i].fd, &buffer, sizeof(buffer), 0);
	if (err < 0)
		throw recvFailed();
	buffer[err] = '\0';
	int cli = getClientIndex(_pollfd[i].fd);
	if (cli == -1)
		return ;
	if (err == 0)
	 	clientDisconnected(i, cli);
	//std::string str(buffer, strlen(buffer) - 1);
	parseBuffer(buffer, cli);
	memset(&buffer, 0, 8192);
}

void Server::new_channel(std::string const & name)
{
	Channel * channel = new Channel(name);
	_channel.insert( std::pair<std::string, Channel *>( name, channel ) );
}

void Server::new_channel(Client & client, std::string const & name)
{
	Channel * channel = new Channel(name, client, *this);
	_channel.insert( std::pair<std::string, Channel *>( name, channel ) );
}

void Server::join_channel(Client & client, std::string const & name)
{
	if (_channel.find(name) != _channel.end())
	{
		_channel[name]->addClient(client, *this);
		client.setChannel(_channel[name]);
	}
}

int	Server::sendAllClients(Channel *channel, int senderFd, std::string num, std::string message) {
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

	int cmd = 10;
	std::string	commands[cmd] = {"PASS", "NICK", "USER", "JOIN", "PART", "QUIT", "OPER", "TOPIC", "KICK", "PRIVMSG"}; //CHANGE number for below 
	int	(Server::*_cPtr[cmd])(Parse parse, int client) = {&Server::cmdPass, &Server::cmdNick, &Server::cmdUser, &Server::cmdJoin, &Server::cmdLeaveChannel, &Server::cmdQuitServer, &Server::cmdOper, &Server::cmdTopic, &Server::cmdKick, &Server::cmdPM};

	for(int i = 0; buffer[i]; i++) {
		if (buffer[i] == '\n')
			buffer[i] = 0; 
	}

	Parse parse(buffer);
	
	for (int i = 0; i < cmd; i++) {
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
	if (parse.args[0].empty())
		_client[c]->sendClient("461", "Not enough parameters");

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

	for (unsigned int i = 0; i < _client.size(); i++)
	{
		if (parse.args[0] == _client[i]->getNickname())
			return (_client[c]->sendClient("433", "Nickname is already in use"), 0);
	}

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
		return (_client[c]->sendClient("451", "You are not registered"), 0);
	
	if (parse.args[0].empty())
		return (_client[c]->sendClient("461", "Not enough parameters"), 0);

	if (_channel.find(parse.args[0]) == _channel.end())
		new_channel(*_client[c], parse.args[0]);
	else
		join_channel(*_client[c], parse.args[0]);

	return (1);
}

int Server::cmdLeaveChannel(Parse parse, int c) {

	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);
	if (_client[c]->getChannel() == NULL)
		return (_client[c]->sendClient("464", "Not in any channel."), 0);
	
	(void) parse;
	std::map<std::string, Channel *>::iterator it;

	for (it = _channel.begin(); it != _channel.end(); ++it)
	{
    	if (it->second == _client[c]->getChannel() || parse.args[0] == "0")
		{
			_client[c]->setChannel(NULL);
			_client[c]->setMode(0);
			it->second->deleteClient(*_client[c], *this);
			return 1;
		}
	}
	return 0;
}

int Server::cmdQuitServer(Parse parse, int c) {
	(void) parse;
	clientDisconnected(_client[c]->getSocket(), c);
	return 1;
}

int Server::cmdOper(Parse parse, int c) {

	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered."), 0);
	if (_client[c]->getMode())
	{
		_client[c]->sendClient("381", "You now have operator rights !");
		return 1;
	}

	(void) parse;
	
	
	if (!this->getOperatorList().empty())
	{
		_client[c]->sendClient("464", "Denied. If you want operator rights, ask an operator.");
		return 0;
	}
	this->getOperatorList().push_back(_client[c]);
	_client[c]->sendClient("381", "You now have operator rights !");
	_client[c]->setMode(1);
	
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
			{
				if (it->second->getTopic() == "")
					_client[c]->sendClient("331", "No topic is set");
				else
					_client[c]->sendClient("332", it->second->getTopic());
			}
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
	
	if (_channel.find(parse.args[1]) == _channel.end())
		_client[c]->sendClient("403", "Channel doesn't exist");
	
	for (unsigned int i = 0; i < _client.size(); i++)
	{
		if (_client[i]->getNickname() == parse.args[0] && _client[i]->getChannel() != NULL)
		{
			// send channel "KICK _client[i] from [channel]"
			_client[c]->sendClient("301", _client[i]->getNickname() + " successfully kicked");
			_client[i]->sendClient("301", "You have been kicked by " + _client[c]->getNickname() + ":" + parse.args[2]);
			_client[i]->getChannel()->deleteClient(*_client[i], *this);
			return 1;
		}
	}
	_client[c]->sendClient("441", "Wrong username or not in that channel.");

	return 0;
}

int Server::cmdPM(Parse parse, int c) {
	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);

	if (parse.args[0][0] == '#' || parse.args[0][0] == '!' || parse.args[0][0] == '&' || parse.args[0][0] == '+')
	{
		std::map<std::string, Channel *>::iterator chan = _channel.find(&parse.args[0][1]);
		if (chan == _channel.end())
			return (_client[c]->sendClient("404", "This channel doesn't exist."), 0);
		else
			return (sendAllClients(chan->second, _client[c]->getSocket(), "301", parse.args[1]), 1);
	}
	
	for (unsigned int i = 0; i < _client.size(); i++)
	{
		if (_client[i]->getNickname() == parse.args[0])
			_client[i]->sendClient("301", parse.args[1]);
		return 1;
	}
	_client[c]->sendClient("401", "Wrong username.");
	return 0;
}

int Server::cmdBot(Parse parse, int c) {
	std::string helpmsg("");

	helpmsg.append("Hi, I am an assistant bot here to walk you through the registration process.\n");
	helpmsg.append("Step #1: PASS [password]\n");
	helpmsg.append("Step #2: NICK [nickname]\n");
	helpmsg.append("Step #3: USER [username] * * [complete name]\n");
	helpmsg.append("Step #4: JOIN [channel]\n");

	(void) parse;
	
	return (_client[c]->sendClient("301", helpmsg), 1);
}

int Server::cmdMode(Parse parse, int c) {
	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);
	if (_client[c]->getChannel() == NULL)
		return (_client[c]->sendClient("442", "Not in any channel."), 0);
	if (parse.args.size() < 1)
		return (_client[c]->sendClient("461", "Not enough parameters"), 0);
	if (!_client[c]->getMode())
		return (_client[c]->sendClient("482", "You don't have the rights for this."), 0);

	if (parse.args[0][0] != '-' && parse.args[0][0] != '+')
		return (_client[c]->sendClient("301", "Wrong argument"), 0);

	(void) parse;

	return 1;
}