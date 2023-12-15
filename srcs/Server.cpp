/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:50:58 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/15 16:26:44 by ldeville         ###   ########.fr       */
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
	int	clen, csock;
	sockaddr_in caddr;
	pollfd pollFd;
	
	clen = sizeof(caddr);
	if ((csock = accept(_serverSock, (struct sockaddr *) &caddr, (socklen_t *) &clen)) < 0)
		throw acceptFailed();

	pollFd.fd = csock;
	pollFd.events = POLLIN;
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
	parseBuffer(buffer, *this, i);
	std::cout << _client[i]->getNickname() << " (" << i << "):" << buffer << std::endl;
}

void Server::new_channel(std::string const & name)
{
	Channel * channel = new Channel(name);
	_channel.insert( std::pair<std::string, Channel *>( name, channel ) );
}

void Server::new_channel(Client const & client, std::string const & name)
{
	Channel * channel = new Channel(name, client);
	_channel.insert( std::pair<std::string, Channel *>( name, channel ) );
}



void	Server::setPass(int i, std::string const & pass) { _client[i]->setPass(pass); };