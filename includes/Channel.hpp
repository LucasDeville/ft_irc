/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 15:50:13 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/20 15:42:48 by bpleutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "common.hpp"

class Channel {
	
private:

	std::string				_name;
	std::string				_topic;
	std::vector<Client>		_client;
	std::vector<Client> 	_operator;
	std::map<const char, int>	_mode;
	Channel();

public:
	
	Channel(std::string const & name);
	Channel(std::string const & name, Client & client);
	~Channel();

	void				addClient(Client & client);
	void				addTopic(std::string str) { _topic = str; };
	void				deleteClient(Client & client);
	std::string const &	getChannelName() const { return _name; };

	std::vector<Client> getClientList() const { return _client; };
	std::vector<Client> getOperatorList() const { return _operator; };
	std::string			getTopic() const { return _topic; };

};

#endif