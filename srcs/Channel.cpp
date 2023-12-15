/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 15:54:31 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/15 16:29:42 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string const & name) : _name(name) {
	
}

Channel::Channel(std::string const & name, Client const & client) : _name(name) {
	addClient(client);
}

Channel::~Channel() {
	
}

void	Channel::addClient(Client const & client) {

	_client.push_back(client);
	std::cout << client.getName() << " joined the channel " << this->_name << std::endl;
}

void	Channel::deleteClient(Client const & client) {

	for(long unsigned int i = 0; i != _client.size(); i++) {
		if (_client[i].getSocket() == client.getSocket())
			_client.erase(_client.begin() + i);
	}
}
