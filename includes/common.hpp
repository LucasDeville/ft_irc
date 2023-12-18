/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 10:25:47 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/18 13:38:06 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <cstring>
#include <poll.h>
#include <map>
#include <vector>
#include <fcntl.h>

class Channel;
class Server;
class Client;

#include "Exception.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

# define DEFAULT_CHANNEL "*"

void	parseBuffer(char buffer[2048], Server const & server, int i);

#endif