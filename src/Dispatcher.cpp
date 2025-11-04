/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 02:22:04 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/29 01:59:51 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dispatcher.hpp"
#include "IReactor.hpp"
#include "Poll.hpp"
#include <sys/poll.h>
#include <utility>
#include <vector>

Dispatcher::Dispatcher() {}

Dispatcher::~Dispatcher() {}

void	Dispatcher::addHandler(int socket, AEventHandler* handler) {
	registry_.insert(std::pair<int, AEventHandler*>(socket, handler));
}

void	Dispatcher::remHandler(int socket) {
	(void)socket;
}

void	Dispatcher::eventLoop() {
	Poll poll;
	std::vector<pollfd> sockets = getSockets();

	poll.wait(sockets, 5000);

	std::vector<pollfd>::iterator socket;
	for (socket = sockets.begin(); socket != sockets.end(); socket++) {
		if (socket->revents == 0) continue;
		registry_.at(socket->fd)->handleEvent(socket->revents);
	}
}

std::vector<pollfd>	Dispatcher::getSockets() {
	std::vector<pollfd> sockets;

	std::map<int, AEventHandler*>::iterator	entry;
	for (entry = registry_.begin(); entry != registry_.end(); entry++) {
		pollfd socket = {.fd = entry->first, .events = POLLIN, .revents = 0};
		sockets.push_back(socket);
	}
	return sockets;
}
