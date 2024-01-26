/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 15:50:13 by ldeville          #+#    #+#             */
/*   Updated: 2024/01/26 11:40:10 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "common.hpp"

class Channel {
	
private:

	std::string				_name;
	std::vector<Client>	_client;
	Channel();

public:
	
	Channel(std::string const & name);
	Channel(std::string const & name, Client & client);
	~Channel();

	void				addClient(Client & client);
	void				deleteClient(Client & client);
	std::string const &	getChannelName() const { return _name; };

};

#endif