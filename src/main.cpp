/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 12:15:18 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/19 19:10:31 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServer.hpp"
#include "Dispatcher.hpp"
#include "Errors.hpp"
#include <csignal>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>

#define ERR_PORT_NB "ERROR: invalid port\n\tvalid port range: [1024-65535]"

bool validParams(long port, std::string const& password)
{
	if (port <= 1023 || port > 65535) {
        std::cerr << ERR_PORT_NB << std::endl;
        return false;
    }
	if (password.empty()) {
        std::cerr << "ERROR: password cannot be set empty" << std::endl;
        return false;
    }
    return true;
}

int	main(int argc, char** argv)
{
	if (argc != 3) {
		std::cout << "Invalid number of arguments" << std::endl; 
		return 1;
	}
	long port = strtol(argv[1], NULL, 10);
	std::string	password = argv[2];
    if (validParams(port, password) == false) return 1;

	try {
        Dispatcher dispatcher;
        IRCServer server(port, password, dispatcher);
        server.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1; // return errno
	}
}
