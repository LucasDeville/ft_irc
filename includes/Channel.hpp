/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 15:50:13 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/15 16:08:58 by ldeville         ###   ########.fr       */
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
	Channel(std::string const & name, Client const & client);
	~Channel();

	void				addClient(Client const & client);
	void				deleteClient(Client const & client);
	std::string const &	getChannelName() const { return _name; };

};

#endif