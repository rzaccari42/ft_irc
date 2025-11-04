/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Acceptor.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 04:00:01 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/29 02:53:17 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Acceptor.hpp"
#include "Connection.hpp"
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

Acceptor::Acceptor(int socket, IReactor& reactor)
	: AEventHandler(socket, reactor) {}

Acceptor::~Acceptor() {}

void	Acceptor::handleEvent(short event) {
	(void)event;
	int	remote;

	remote = accept(socket_, NULL, NULL);
	fcntl(remote, F_SETFL, fcntl(remote, F_GETFL) | O_NONBLOCK);
	Connection* connection = new Connection(remote, reactor_);
	reactor_.addHandler(remote, connection);
	std::cout << "new connection established" << std::endl;
}
