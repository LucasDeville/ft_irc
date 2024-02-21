/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 15:54:31 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/21 10:34:24 by ldeville         ###   ########.fr       */
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

Channel::Channel(std::string const & name, Client & client) : _name(name), _topic("") {
	addClient(client);
	_mode.insert(std::pair<const char, int>('i', 0));
	_mode.insert(std::pair<const char, int>('t', 0));
	_mode.insert(std::pair<const char, int>('k', 0));
	_mode.insert(std::pair<const char, int>('o', 0));
	_mode.insert(std::pair<const char, int>('l', 0));
}

Channel::~Channel() {
	
}

void	Channel::addClient(Client & client) {

	_client.push_back(client);
	std::cout << client.getNickname() << " joined the channel " << this->_name << std::endl;
	client.sendClient("", ("JOIN :" + _name + "\n"));
/*
	sendClient("", _client[c]->getUserPrefix() + "JOIN " + ChannelName + "\n");
	_client[c]->sendClient("332", this->_client[c]->getNickName(), ChannelName + " :" + it->second->getTopic()));
	_client[c]->sendClient("353", this->_client[c]->getNickName() + " = " + ChannelName, it->second->listAllUsers()));
	_client[c]->sendClient("353", this->_client[c]->getNickName() + " " + ChannelName, ":End of NAMES list"));
*/
}

void	Channel::deleteClient(Client & client) {

	for(long unsigned int i = 0; i != _client.size(); i++) {
		if (_client[i].getSocket() == client.getSocket())
			_client.erase(_client.begin() + i);
	}
}
