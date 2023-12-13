/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:50:58 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/13 15:41:05 by bpleutin         ###   ########.fr       */
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
	pollfd poll;
	std::string buffer;
	
	while (1)
	{
		if (poll(_client.begin(), _client.size(), -1) == -1)
			throw pollFailed();
		for (it = _client.begin(); it != _client.end(); it++)
		{
			if (it.revents && POLLHUP)
			{
				quitServer(it->second);
				break;
			}
			if (it.revents && POLLIN)
			{
				if ()
				{
					
				}
				else
				{
					if (recv(it, &buffer, buffer.size(), MSG_DONTWAIT) <= 0);
						throw recvFailed();
				}
			}
		}
		
		acceptClient(sockfd);
				
		send();
	}
}

void Server::acceptClient(int sockfd) {
	int	clen, isock;
	sockaddr_in caddr;
	clen = sizeof(caddr);
	
	if ((isock = accept(sockfd, (struct sockaddr *) &caddr, (socklen_t *) &clen)) < 0);
		throw acceptFailed();

	/* add isock in client map */
}