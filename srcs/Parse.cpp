/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 15:54:31 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/21 10:36:22 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parse.hpp"

Parse::Parse(std::string & arg) : args(), cmd(""), invalid(false) {

	size_t	i = 0;
	size_t	j = 0;
	bool done = false;

	if (arg[i] == ' ' || !arg[i])
		invalid = true;
	else {
		while (arg[i])
		{
			if (arg[i] == ' ')
			{
				if (arg[i + 1] == ' ') {
					invalid = true;
					break;
				}
				args.push_back(arg.substr(j, i - j));
				while (arg[i] == ' ')
					i++;
				j = i;
			}
			if (arg[i] == ':')
			{
				if (arg[i - 1] != ' ') {
					invalid = true;
					break;
				}
				args.push_back(arg.substr(i + 1, arg.length() - i));
				cmd = args[0];
				args.erase(args.begin());
				done = true;
				break;
			}
			i++;
		}
		if (!done) {
			if (i && arg[j])
				args.push_back(arg.substr(j, i - j));
			cmd = args[0];
			args.erase(args.begin());
		}
	}
}

