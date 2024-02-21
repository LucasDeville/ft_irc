/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 10:25:47 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/21 11:31:11 by ldeville         ###   ########.fr       */
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
class Parse;
class Server;
class Client;

#include "Exception.hpp"
#include "Parse.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

# define DEFAULT_CHANNEL "*"

#endif