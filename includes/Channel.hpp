/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 15:50:13 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/22 15:47:47 by bpleutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "common.hpp"

class Channel {
	
private:

	std::string				_name;
	std::string				_topic;
	std::string				_key;
	int						_limit;
	int						_users;
	std::vector<Client>		_client;
	std::map<const char, int>	_mode;
	Channel();

public:
	
	Channel(std::string const & name);
	Channel(std::string const & name, Client & client, Server & server);
	~Channel();

	std::string			listAllUsers() const;
	int					getClientNum() const;
	void				addClient(Client & client, Server & server);
	void				addTopic(std::string str) { _topic = str; };
	void				deleteClient(Client & client, Server & server);
	std::string const &	getChannelName() const { return _name; };
	std::string const & getKey() const { return _key; };
	int	const & getLimit() const { return _limit; };
	int const & getUsers() const { return _users; };
	int const & getMode(char c) const { return _mode.find(c)->second; };

	std::vector<Client> getClientList() const { return _client; };
	std::string			getTopic() const { return _topic; };

	std::vector<Client>	getAllClients() const;
	
	void	setMode(const char c, int state) { _mode[c] = state; };
	void	setKey(std::string k) { _key = k; };
	void	setLimit(int nb) { _limit = nb; };

	void	addUser() { _users++; };
	void	rmUser() { _users--; };

};

#endif