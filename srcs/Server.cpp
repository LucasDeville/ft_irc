/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:50:58 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/12 14:53:01 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {
	
}

Server::Server(int port, std::string passwd) : _port(port), _passwd(passwd) {

}

Server::~Server() {
	
}