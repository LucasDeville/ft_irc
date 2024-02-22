/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:52:29 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/22 15:29:43 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {

}

Client::Client(int socket) : _hostname("localhost"), _mode(0), _socket(socket), _auth(false), _registered(false), _channel(NULL) {

}

Client::~Client() {

}


void    Client::sendWelcome() const
{
    std::string paquet = ".----------------.  .----------------.  .----------------.  .----------------.\n";
    paquet.append("| .--------------. || .--------------. || .--------------. || .--------------. |\n");
    paquet.append("| |    ______    | || |  ________    | || |    _____     | || |  ____  ____  | |\n");
    paquet.append("| |   / ____ `.  | || | |_   ___ `.  | || |   / ___ `.   | || | |_  _||_  _| | |\n");
    paquet.append("| |   `'  __) |  | || |   | |   `. \\ | || |  |_/___) |   | || |   \\ \\  / /   | |\n");
    paquet.append("| |   _  |__ '.  | || |   | |    | | | || |   .'____.'   | || |    \\ \\/ /    | |\n");
    paquet.append("| |  | \\____) |  | || |  _| |___.' / | || |  / /____     | || |    _|  |_    | |\n");
    paquet.append("| |   \\______.'  | || | |________.'  | || |  |_______|   | || |   |______|   | |\n");
    paquet.append("| |              | || |              | || |              | || |              | |\n");
    paquet.append("| '--------------' || '--------------' || '--------------' || '--------------' |\n");
    paquet.append("'----------------'  '----------------'  '----------------'  '----------------' \n\n");
    paquet.append("Welcome to the IRC 3D2Y server ! You can use the bot with the command 'HELP' if you need it.\n");

    if (send(_socket, paquet.c_str(), paquet.length(), 0) < 0)
        throw(std::out_of_range("Error while sending to the client."));
}

void    Client::sendClient(std::string num, std::string nickname, std::string str) 
{
    std::string paquet = num + " " + nickname + " :" + str + "\n";
    if (send(_socket, paquet.c_str(), paquet.length(), 0) < 0)
        throw(std::out_of_range("Error while sending to the client."));
}

void    Client::sendClient(std::string num, std::string str) 
{
    std::string paquet = num + " " + (_nickname.empty() ? "*" : _nickname) + " :" + str + "\n";
    if (send(_socket, paquet.c_str(), paquet.length(), 0) < 0)
        throw(std::out_of_range("Error while sending to the client."));
}

void Client::notRegistered() {

	if (_nickname.empty() || _username.empty() || _passwd.empty())
		return;
	_registered = true;
	sendClient("001", ("You are now registered to the IRC 3D2Y server, " +_nickname + "!" + _username + "@" + _hostname ));
}

void	Client::setChannel(Channel *channel) {
	_channel = channel;
}

Channel		*Client::getChannel() const {
	return _channel;
}