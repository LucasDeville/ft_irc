/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:33:17 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/12 11:22:58 by bpleutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.hpp"

int	main(int argc, char **argv) {

    if (argc != 3 && atoi(argv[2]) > 65535 && atoi(argv[2]) < 0)
    {
        std::cout << "Error: could not start IRC." << std::endl;
        return (-1);
    }
    return (0);
}