/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:33:17 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/13 11:35:01 by bpleutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.hpp"
#include "Server.hpp"

int	main(int argc, char **argv) {

    if (argc != 3 || (atoi(argv[1]) > 65535 || atoi(argv[1]) < 1024))
    {
        std::cout << "Error: could not start IRC." << std::endl;
        return (-1);
    }
    Server server(atoi(argv[1]), argv[2]);
    return (0);
}