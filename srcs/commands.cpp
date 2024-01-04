/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 10:39:27 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/15 15:21:16 by bpleutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.hpp"

// static void	initUser(char buffer[2048], Server const & server, int i) {
	
// 	int i = 0;
// 	for (i; buffer[i] && buffer[i] != '\n'; i++);
// 	if (buffer[i] && buffer[i] == '\n')
// 		i++;
// 	// server.setPass(i, substr(&buffer[i], 5));
// }

// static void	setUser(char buffer[2048], Server const & server) {
	

// }

// static void	setPass(char buffer[2048], Server const & server) {
	

// }

void	parseBuffer(char buffer[2048], Server const & server, int i) {

	std::cout << "|" << buffer << "|" << std::endl;
	(void)server;
	(void)i;
	if (!strncmp(buffer, "CAP ", 4))
		return ;
		// initUser(buffer, server, i);
	// else if (!strncmp(buffer + 5, "USER ", 5))
	// 	setUser(buffer, server);
	// else if (!strncmp(buffer, "PASS ", 5))
	// 	setPass(buffer + 5, server);
}
