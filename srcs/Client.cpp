/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:52:29 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/20 21:15:06 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {

}

Client::Client(int socket) : _hostname("localhost"), _mode(0), _socket(socket), _auth(false), _registered(false), _channel(NULL) {

}

Client::~Client() {

}

void    Client::sendClient(std::string num, std::string str) 
{
    std::string paquet = ":" + num + " " + (_nickname.empty() ? "*" : _nickname) + " :" + str + "\n";
    std::cout << "---> " << paquet << std::endl;
    if (send(_socket, paquet.c_str(), paquet.length(), 0) < 0)
        throw(std::out_of_range("Error while sending to the client."));
}

void Client::notRegistered() {

	if (_nickname.empty() || _username.empty() || _passwd.empty()) {
		//sendClient("451", "Waiting for registration...\n");
		return;
	}
	_registered = true;
	sendClient("001", ":Welcome to the 42 IRC Network !\n");
}

void	Client::setChannel(Channel *channel) {
	_channel = channel;
}

Channel		*Client::getChannel() const {
	return _channel;
}