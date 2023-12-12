/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:33:17 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/12 10:34:31 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.hpp"

int	main(int argc, char **argv) {

    (void)argv;
    if (argc != 3)
    {
        std::cout << "Error: could not start IRC." << std::endl;
        return (-1);
    }
    return (0);
}