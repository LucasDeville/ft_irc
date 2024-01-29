/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 15:54:31 by ldeville          #+#    #+#             */
/*   Updated: 2024/01/26 11:40:56 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string const & name) : _name(name) {
	
}

Channel::Channel(std::string const & name, Client & client) : _name(name) {
	addClient(client);
}

Channel::~Channel() {
	
}

void	Channel::addClient(Client & client) {

	_client.push_back(client);
	std::cout << client.getNickname() << " joined the channel " << this->_name << std::endl;
	client.sendClient(("JOIN :" + _name + "\n"));
}

void	Channel::deleteClient(Client & client) {

	for(long unsigned int i = 0; i != _client.size(); i++) {
		if (_client[i].getSocket() == client.getSocket())
			_client.erase(_client.begin() + i);
	}
}
