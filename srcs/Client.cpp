/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:52:29 by ldeville          #+#    #+#             */
/*   Updated: 2024/01/25 14:52:20 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {

}

Client::Client(int socket) : _socket(socket), _registered(false) {

}

Client::~Client() {

}

void    Client::sendClient(std::string str) 
{
    std::string prefix = _nickname + (_username.empty() ? "Server" : "!" + _username) + (_hostname.empty() ? "" : "@" + _hostname);
    std::string paquet = ":" + prefix + " " + str + "\r\n";
    std::cout << "---> " << paquet << std::endl;
    if (send(_socket, paquet.c_str(), paquet.length(), 0) < 0)
        throw(std::out_of_range("Error while sending to the client."));
}

void Client::notRegistered() {

	if (_nickname.empty() || _username.empty() || _hostname.empty() || _passwd.empty()) {
		sendClient("Waiting for registration...\n");
		return;
	}
	_registered = true;
	sendClient("001 user :Welcome to the Internet Relay Network 127.0.0.1!\n");
}
