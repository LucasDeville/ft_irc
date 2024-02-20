/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:43:05 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/20 15:18:52 by bpleutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "common.hpp"

class Client {

private:

	std::string	_username;
	std::string	_realname;
	std::string	_nickname;
	std::string	_hostname;
	std::string	_passwd;
	int			_mode;
	int			_socket;
	bool		_registered;
	Channel*	_channel;
	Client();

public:

	Client(int socket);
	~Client();

	std::string const &	getUsername() const { return _username; };
	std::string const &	getRealname() const { return _realname; };
	std::string const &	getNickname() const { return _nickname; };
	std::string const &	getHostname() const { return _hostname; };
	std::string const &	getPassword() const { return _passwd; };
	std::string			getPrefix();
	bool const &		getRegistered() const { return _registered; };
	int const &			getMode() const { return _mode; };
	int const &			getSocket() const { return _socket; };
	
	void				setUser(std::string const & user) { _username = user; };
	void				setRealname(std::string const & real) { _realname = real; };
	void				setNickname(std::string const & nick) { _nickname = nick; };
	void				setPass(std::string const & pass) { _passwd = pass; };
	void				setMode(int m) { _mode = m; };
	
	void				notRegistered();
	void				sendClient(std::string str);
	void				setChannel(Channel *channel);
	Channel				*getChannel(void) const;
};

#endif