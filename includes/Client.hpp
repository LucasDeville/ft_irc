/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:43:05 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/15 16:10:04 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "common.hpp"

class Client {

private:

	std::string	_username;
	std::string	_nickname;
	std::string	_name;
	std::string	_passwd;
	int			_mode;
	int			_socket;
	bool		_registered;
	Client();

public:

	Client(int socket);
	~Client();

	std::string const &	getUsername() const { return _username; };
	std::string const &	getNickname() const { return _nickname; };
	std::string const &	getName() const { return _name; };
	std::string const &	getPassword() const { return _passwd; };
	int const &			getMode() const { return _mode; };
	int const &			getSocket() const { return _socket; };
	void				setUser(std::string const & user) { _username = user; };
	void				setPass(std::string const & pass) { _passwd = pass; };
};

#endif