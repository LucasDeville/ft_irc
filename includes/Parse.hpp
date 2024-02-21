/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parse.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpleutin <bpleutin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 15:50:13 by ldeville          #+#    #+#             */
/*   Updated: 2024/02/20 15:42:48 by bpleutin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_HPP
#define PARSE_HPP

#include "common.hpp"

class Parse {
	
private:
	Parse() { };

public:
	
	std::vector<std::string>				args;
	std::string								cmd;
	bool									invalid;


	Parse(std::string & arg);
	~Parse() { };
};

#endif