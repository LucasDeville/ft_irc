/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 15:54:31 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/22 15:48:18 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string const & name) : _name(name), _topic("") {
	_mode.insert(std::pair<const char, int>('i', 0));
	_mode.insert(std::pair<const char, int>('t', 0));
	_mode.insert(std::pair<const char, int>('k', 0));
	_mode.insert(std::pair<const char, int>('o', 0));
	_mode.insert(std::pair<const char, int>('l', 0));
}

Channel::Channel(std::string const & name, Client & client, Server & server) : _name(name), _topic("") {
	addClient(client, server);
	_mode.insert(std::pair<const char, int>('i', 0));
	_mode.insert(std::pair<const char, int>('t', 0));
	_mode.insert(std::pair<const char, int>('k', 0));
	_mode.insert(std::pair<const char, int>('o', 0));
	_mode.insert(std::pair<const char, int>('l', 0));
}

Channel::~Channel() {
	
}

std::string		Channel::listAllUsers() const
{
	std::string		str(":");
	std::vector<Client>::const_iterator it = _client.begin();
	while (it != _client.end())
	{
		str.append("@" + it->getNickname() + " ");
		it++;
	}
	return (str);
}

int		Channel::getClientNum() const
{
	int i = 0;

	std::vector<Client>::const_iterator it = _client.begin();
	while (it != _client.end()) {
		i++;
		it++;
	}
	std::cout << "getClientNum = " << i << std::endl;
	return (i);
}

void	Channel::addClient(Client & client, Server & server) {

	_client.push_back(client);
	client.sendClient("", "", ("JOIN #" + _name));
	client.sendClient("332", "Server", (_name + " :" + _topic));
	client.sendClient("353", "Server",(" = " + _name, listAllUsers()));
	client.sendClient("353", "Server",(" " + _name + " :End of NAMES list."));
	server.sendAllClients(this, client.getSocket(), ("#" +_name), "301", (client.getNickname() + " join the channel"));
}

void	Channel::deleteClient(Client & client, Server & server) {

	for(long unsigned int i = 0; i != _client.size(); i++) {
		if (_client[i].getSocket() == client.getSocket()) {
			server.sendAllClients(this, _client[i].getSocket(), ("#" +_name), "301", (_client[i].getNickname() + " just left the channel"));
			_client.erase(_client.begin() + i);
			break ;
		}
	}
}

std::vector<Client>	Channel::getAllClients() const {
	return (_client);
}
