/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:50:58 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/22 16:08:03 by bpleutin         ###   ########.fr       */
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

void Server::checkChan(Channel * chan) {

	std::map<std::string, Channel *>::iterator it = _channel.find(chan->getChannelName());
	if (it != _channel.end()) {
		if (it->second->getClientNum() <= 0)
			delete it->second;
		_channel.erase(it);
	}

}

void Server::clientDisconnected(long unsigned int i, int cli) {
	close(_client[cli]->getSocket());
	_pollfd.erase(_pollfd.begin() + i);
	if (_client[cli]->getChannel() != NULL) {
		_client[cli]->getChannel()->deleteClient(*_client[cli], *this);
		_client[cli]->getChannel()->rmUser();
		checkChan(_client[cli]->getChannel());
	}
	std::cout << "IRC: Client '" << _client[i]->getNickname() << "' connection closed." << std::endl;
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

unsigned long int Server::findPoolId(int fd) {

	unsigned long int index = 0;

	while (_pollfd[index].fd > 0) {
		if (_pollfd[index].fd == fd)
			return index;
		index++;
	}
	return index;
}

void Server::handleInput(unsigned long int i) {
	char buffer[8192];
	int err = recv(_pollfd[i].fd, &buffer, sizeof(buffer), 0);
	int cli = getClientIndex(_pollfd[i].fd);
	if (cli == -1)
		return ;
	if (err <= 0) {
		
		if (err < 0)
			throw recvFailed();
		clientDisconnected(i, cli);
		return ;
	}
	buffer[err] = '\0';
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
		_channel[name]->addUser();
		client.setChannel(_channel[name]);
	}
}

int	Server::sendAllClients(Channel *channel, int senderFd, std::string nickname, std::string num, std::string message) {
	std::vector<Client> clients = channel->getAllClients();
	std::vector<Client>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if (senderFd != it->getSocket())
			it->sendClient(num, nickname, message);
		it++;
	}
	return (1);
}

void	Server::parseBuffer(std::string buffer, int client) {

	int cmd = 14;
	std::string	commands[cmd] = {"PASS", "NICK", "USER", "JOIN", "PART", "QUIT", "OPER", "TOPIC", "KICK", "PRIVMSG", "SENDFILE", "GETFILE", "INVITE", "HELP"}; //CHANGE number for below 
	int	(Server::*_cPtr[cmd])(Parse parse, int client) = {&Server::cmdPass, &Server::cmdNick, &Server::cmdUser, &Server::cmdJoin, &Server::cmdLeaveChannel, &Server::cmdQuitServer, &Server::cmdOper, &Server::cmdTopic, &Server::cmdKick, &Server::cmdPM, &Server::cmdSendF, &Server::cmdGetF, &Server::cmdInv, &Server::cmdBot};

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

	/*if (_channel.find(parse.args[0])->second->getMode('i') == 1)
	{
		unsigned int i;
		for (i = 0; i < _client.size(); i++)
			if (_channel.find(parse.args[0])->second->getInvites()[i] == _client[c])
				break;
		if (i == _channel.find(parse.args[0])->second->getInvites().size())
			return (_client[c]->sendClient("", "Invite-only channel"), 0);
	}*/

	if (_channel.find(parse.args[0])->second->getLimit() != -1
		|| _channel.find(parse.args[0])->second->getUsers() >= _channel.find(parse.args[0])->second->getLimit())
		return (_client[c]->sendClient("471", "Server", "Channel is full"), 0);

	if (_client[c]->getChannel() != NULL) {
		_client[c]->getChannel()->deleteClient(*_client[c], *this);
		_client[c]->setOper(0);
		_client[c]->setChannel(NULL);
	}
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
			it->second->rmUser();
			it->second->deleteClient(*_client[c], *this);
			return 1;
		}
	}
	return 0;
}

int Server::cmdQuitServer(Parse parse, int c) {
	(void) parse;
	clientDisconnected(findPoolId(_client[c]->getSocket()), c);
	return 1;
}

int Server::cmdOper(Parse parse, int c) {

	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered."), 0);
	if (_client[c]->getOper())
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
	_client[c]->setOper(1);
	
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
	if (_channel.find(parse.args[0])->second->getMode('t') == 1 && !_client[c]->getOper())
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
	if (!_client[c]->getOper())
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
		else {
			if (_client[c]->getChannel()->getChannelName().compare(&parse.args[0][1]) != 0)
				return (_client[c]->sendClient("404", "You are not in that channel !"), 1);
			else	
				return (sendAllClients(chan->second, _client[c]->getSocket(), _client[c]->getNickname(), "301", parse.args[1]), 1);
		}
	}
	
	for (unsigned int i = 0; i < _client.size(); i++)
	{
		if (_client[i]->getNickname() == parse.args[0]) {
			_client[i]->sendClient("301", _client[c]->getNickname(), parse.args[1]);
			return 1;
		}
	}
	_client[c]->sendClient("401", "Wrong username.");
	return 0;
}

int Server::cmdGetF(Parse parse, int c) {
	if (!_client[c]->getRegistered())
		return (_client[c]->sendClient("451", "You are not registered"), 0);
	if (parse.args.size() < 2)
		return (_client[c]->sendClient("461", "Not enough parameters"), 0);
	if (_files.find(parse.args[0]) == this->_files.end())
		return (_client[c]->sendClient("995", "No such file on the server"), 0);
		
	File file(this->_files.find(parse.args[0])->second);
	if (file.nickreceiver != this->_client[c]->getNickname())
		return (_client[c]->sendClient("994", "Permission Denied"), 0);
	
	std::fstream	ofs((parse.args[1] + "/" + parse.args[0]).c_str(), std::fstream::out);
	std::fstream	ifs(file.path.c_str(), std::fstream::in);
	if (ofs.is_open())
		ofs << ifs.rdbuf();
	this->_files.erase(file.filename);
	
	return 1;
}

int Server::cmdSendF(Parse parse, int c) {
	if (!_client[c]->getRegistered())
		return (_client[c]->sendClient("451", "You are not registered"), 0);
	if (parse.args.size() < 2)
		return (_client[c]->sendClient("461", "Not enough parameters"), 0);
	for (unsigned int i = 0; i < _client.size(); i++)
	{
		if (_client[i]->getNickname() == parse.args[0])
		{
			std::fstream ifs(parse.args[1].c_str(), std::fstream::in);
			_client[i]->sendClient("301", parse.args[1].c_str());
			if (ifs.fail())
				return (_client[c]->sendClient("999", "Invalid file path"), 0);
			size_t	pos = parse.args[1].find_last_of('/');
			std::string	filename = parse.args[1].substr(pos + 1);
			File file(filename, parse.args[1], this->_client[c]->getNickname(), parse.args[0]);
			if (this->_files.find(filename) != this->_files.end())
				return (_client[c]->sendClient("996", "File with this name already exists"), 0);
			this->_files.insert(std::pair<std::string, File>(filename, file));
			
			for (unsigned int i = 0; i < _client.size(); i++)
			{
				if (_client[i]->getNickname() == parse.args[0])
					_client[i]->sendClient("301", _client[c]->getNickname() + " wants to send you a file called " + filename);
			}
			return 1;
		}
	}
	return (_client[c]->sendClient("401", "No such nick/channel"), 0);
}

int Server::cmdInv(Parse parse, int c) {
	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);
	if (_client[c]->getChannel() == NULL)
		return (_client[c]->sendClient("442", "Not in any channel."), 0);
	if (parse.args.size() < 2)
		return (_client[c]->sendClient("461", "Not enough parameters"), 0);
	if (!_client[c]->getOper())
		return (_client[c]->sendClient("482", "You don't have the rights for this."), 0);

	
	for (unsigned int i = 0; i < _client.size(); i++)
	{
		if (_client[i]->getNickname() == parse.args[0])
		{
			if (_client[i]->getChannel()->getChannelName().compare(parse.args[1]) != 0)
			{
				_client[c]->sendClient("443", "User is already on channel");
				return 1;
			}
			_client[i]->sendClient("301", "User " + _client[c]->getNickname() + " invites you to channel " + parse.args[1]);
			_client[c]->sendClient("341", "Invitation sent successfully.");
		}
	}
	_client[c]->sendClient("401", "Wrong username.");
	return 0;
}

int Server::cmdBot(Parse parse, int c) {
	std::string helpmsg("");

	if (!_client[c]->getRegistered()) {
		helpmsg.append("Hi, I am an assistant bot here to walk you through the registration process.\n");
		helpmsg.append("Step #1: PASS [password]\n");
		helpmsg.append("Step #2: NICK [nickname]\n");
		helpmsg.append("Step #3: USER [username] * * [complete name]\n");
	}
	else {
		helpmsg.append("You can navigate throught the server with different commande like :\n");
		helpmsg.append("JOIN [channel]\n");
		helpmsg.append("PRIVMSG [nickname or #channel] : [message]\n");
	}

	(void) parse;
	
	return (_client[c]->sendClient("301", helpmsg), 1);
}

int Server::cmdMode(Parse parse, int c) {
	if (!_client[c]->getRegistered())
		return(_client[c]->sendClient("451", "You are not registered"), 0);
	if (_client[c]->getChannel() == NULL)
		return (_client[c]->sendClient("442", "Not in any channel."), 0);
	if (parse.args.size() < 2)
		return (_client[c]->sendClient("461", "Not enough parameters"), 0);
	if (!_client[c]->getOper())
		return (_client[c]->sendClient("482", "You don't have the rights for this."), 0);

	std::map<std::string, Channel *>::iterator chan = _channel.find(&parse.args[0][0]);
	if (chan == _channel.end())
			return (_client[c]->sendClient("404", "This channel doesn't exist."), 0);
	
	unsigned int i = 0;

	if (parse.args[1][i] != '-' && parse.args[1][i++] != '+')
		return (_client[c]->sendClient("301", "Wrong argument"), 0);

	(void) parse;

	int signIdx = 0;

	loop:
	switch (parse.args[1][i])
	{
		case 'i':
		{
			if (parse.args[1][signIdx] == '-')
				chan->second->setMode('i', 0);
			else
				chan->second->setMode('i', 1);
			i++;
			goto loop;
		}
		case 't':
		{
			if (parse.args[1][signIdx] == '-')
				chan->second->setMode('t', 0);
			else
				chan->second->setMode('t', 1);
			i++;
			goto loop;
		}
		case 'k':
		{
			if (parse.args[1][signIdx] == '-')
				chan->second->setKey("");
			else
			{
				if (!chan->second->getKey().empty())
					return (_client[c]->sendClient("467", "Server", "Key already set"), 0);
				else
				{
					if (parse.args.size() < 3)
						return (_client[c]->sendClient("461", "Not enough parameters"), 0);
					chan->second->setKey(parse.args[2]);
				}
			}
			i++;
			goto loop;
		}
		case 'o':
		{
			if (parse.args.size() < 3)
				return (_client[c]->sendClient("461", "Not enough parameters"), 0);
			
			unsigned int j;
			for (j = 0; j < _client.size(); j++)
			{
				if (_client[j]->getNickname() == parse.args[2])
					break;
			}
			if (j == _client.size())
				return (_client[c]->sendClient("401", "No such nickname"), 0);
	
			if (parse.args[1][signIdx] == '-')
				_client[j]->setOper(0);
			else
				_client[j]->setOper(1);
			i++;
			goto loop;
		}
		case 'l':
		{
			if (parse.args[1][signIdx] == '-')
				chan->second->setLimit(-1);
			else
			{
				if (parse.args.size() < 3)
					return (_client[c]->sendClient("461", "Not enough parameters"), 0);	
				if (std::atoi(parse.args[2].c_str()) == 0)
					return (_client[c]->sendClient("301", "Server", "Invalid limit number"), 0);
				chan->second->setLimit(std::atoi(parse.args[2].c_str()));
			}
			i++;
			goto loop;
		}
		case '+':
			signIdx = i;
			i++;
			goto loop;
		case '-':
			signIdx = i;
			i++;
			goto loop;
		default:
		{
			if (i == parse.args[1].size() - 1)
				break;
			else
				return (_client[c]->sendClient("472", parse.args[1][i] + ": unknown mode."), 0);
		}
	}

	return 1;
}
