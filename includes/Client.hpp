/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:43:05 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/12 14:52:41 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "common.hpp"

class Client {

private:

	std::string	_username;
	std::string	_nickname;

public:

	Client();
	~Client();

	std::string const &	getUsername() const { return _username; };
	std::string const &	getNickname() const { return _nickname; };
};

#endif