/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:52:29 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/20 16:13:19 by bpleutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {

}

Client::Client(int socket) : _hostname("localhost"), _mode(0), _socket(socket), _registered(false), _channel(NULL) {

}

Client::~Client() {

}

void    Client::sendClient(std::string str) 
{
    std::string prefix = _nickname + (_username.empty() ? "" : "!" + _username) + (_hostname.empty() ? "" : "@" + _hostname);
    std::string paquet = ":" + prefix + " " + str + "\r\n";
    std::cout << "---> " << paquet << std::endl;
    if (send(_socket, paquet.c_str(), paquet.length(), 0) < 0)
        throw(std::out_of_range("Error while sending to the client."));
}

void Client::notRegistered() {

	if (_nickname.empty() || _username.empty() || _passwd.empty()) {
		sendClient("Waiting for registration...\n");
		return;
	}
	_registered = true;
	sendClient("001 " + getUsername() + " :Welcome to the 42 IRC Network !\n");
}

std::string Client::getPrefix()
{
    std::string str = ":" + _nickname + (_username.empty() ? "" : "!" + _username) + (_hostname.empty() ? "" : "@" + _hostname);
    return str;
}

void	Client::setChannel(Channel *channel) {
	_channel = channel;
}

Channel		*Client::getChannel() const {
	return _channel;
}