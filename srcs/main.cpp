/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:33:17 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/13 12:12:13 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.hpp"
#include "Server.hpp"

int	main(int argc, char **argv) {

    if (argc != 3 || (atoi(argv[1]) > 65535 || atoi(argv[1]) < 1))
    {
        std::cout << "Error: could not start IRC." << std::endl;
        return (-1);
    }
    Server server(atoi(argv[1]), argv[2]);
    try {
        server.createServer();
    }
    catch (const std::exception &e){
        std::cout << e.what() << std::endl;
    }
    return (0);
}
