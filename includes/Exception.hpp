/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeville <ldeville@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 12:09:27 by ldeville          #+#    #+#             */
/*   Updated: 2023/12/13 12:10:28 by ldeville         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include "iostream"

class bindingFailed : public std::exception
{ 
	public: virtual const char *what() const throw() 
		{ return ("createServer : binding failed."); } 
};

class socketFailed : public std::exception
{ 
	public: virtual const char *what() const throw() 
		{ return ("createServer : socket failed."); } 
};

class listenFailed : public std::exception
{ 
	public: virtual const char *what() const throw() 
		{ return ("createServer : listen failed."); } 
};

#endif