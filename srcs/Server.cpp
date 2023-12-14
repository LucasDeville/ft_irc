/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:50:58 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/14 11:39:29 by bpleutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {
	
}

Server::Server(int port, std::string passwd) : _port(port), _passwd(passwd) {

}

Server::~Server() {
	
}

void Server::createServer() {
	sockaddr_in saddr;
	int sockfd = socket(AF_INET, SOCK_STREAM, NULL);
	
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
	serverLoop(sockfd);
}

void Server::serverLoop(int sockfd) {
	std::map<pollfd, Client *>::iterator it;
	pollfd pollFd;
	std::vector<pollfd> keys;
    for (it = _client.begin(); it != _client.end(); it++)
        keys.push_back(it->first);
	
	while (1)
	{
		if (poll(&keys[0], _client.size(), -1) == -1)
			throw pollFailed();
		for (it = _client.begin(); it != _client.end(); it++)
		{
			pollFd = it->first;
			if (pollFd.revents && POLLHUP)
			{
				quitServer(it->second);
				break;
			}
			if (pollFd.revents && POLLIN)
			{
				if (pollFd.fd == sockfd)
					acceptClient(sockfd);
				else
					handleInput(sockfd);
			}
		}
	}
}

void Server::acceptClient(int sockfd) {
	int	clen, csock;
	sockaddr_in caddr;
	pollfd pollFd;
	
	clen = sizeof(caddr);
	if ((csock = accept(sockfd, (struct sockaddr *) &caddr, (socklen_t *) &clen)) < 0);
		throw acceptFailed();

	pollFd.fd = csock;
	pollFd.events = POLLIN;
	pollFd.revents = 0;

	/* create new Client */

	_client.insert({pollFd, NULL});
}

void Server::handleInput(std::map<pollfd, Client *>::iterator it, int sockfd) {
	std::string buffer;
	
	if (int err = recv(it->second->getSocket(), &buffer, buffer.size(), 0) < 0)
		throw recvFailed();
	else if (err == 0)
		clientDisconnected();
	std::cout << it->second->getNickname() << buffer << std::endl;
}