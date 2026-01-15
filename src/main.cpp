/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 12:15:18 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/15 00:42:05 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"
#include "Dispatcher.hpp"
#include "Acceptor.hpp"
#include "Errors.hpp"
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>

#define ERR_PORT_NB "Error: invalid port\n\tvalid port range: [1024-65535]"

bool	isvalid_port(int port) {
	if (port <= 1023 || port > 65535) {
		std::cout << ERR_PORT_NB << std::endl; 
		return false;
	}
	return true;
}

bool	isvalid_password(std::string password) {
	if (password.empty()) {
		std::cout << "Error: password can't be empty" << std::endl; 
		return false;
	}
	return true;
}

int	main(int argc, char** argv) {
	if (argc != 3) {
		std::cout << "Invalid number of arguments" << std::endl; 
		return -1;
	}

    
	long port = strtol(argv[1], NULL, 10);
	std::string	password = argv[2];
	if (isvalid_port(port) == false) return -1;
	if (isvalid_password(password) == false) return -1;

	try {
		Listener listener(static_cast<uint16_t>(port));

		Dispatcher dispatcher;

		Acceptor* acceptor = new Acceptor(listener.getSockfd(), dispatcher);
		dispatcher.addHandler(listener.getSockfd(), acceptor);

		do { dispatcher.eventLoop(); } while (1);

	} catch (SocketError const& error) {
		std::cerr << error.what() << std::endl;
		return 1; // return errno
	}
	
}
