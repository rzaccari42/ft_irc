/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 02:22:04 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/15 12:52:57 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dispatcher.hpp"
#include "EventHandler.hpp"
#include "IReactor.hpp"
#include "Poll.hpp"
#include <sys/poll.h>
#include <utility>
#include <vector>

Dispatcher::Dispatcher() {}

Dispatcher::~Dispatcher() {}

void	Dispatcher::addHandler(int socket, AEventHandler* handler)
{
	registry_.insert(std::pair<int, AEventHandler*>(socket, handler));
}

void	Dispatcher::remHandler(int socket)
{
    if (dispatching_) {
        pendingRem_.push_back(socket);
        return;
    }
    std::map<int, AEventHandler*>::iterator entry = registry_.find(socket);
    if (entry == registry_.end()) return;
    delete entry->second;
    registry_.erase(entry);
}

void	Dispatcher::eventLoop()
{
    dispatching_ = true;

	Poll poll;
	std::vector<pollfd> sockets = getSockets();
	poll.wait(sockets, 5000);

	std::vector<pollfd>::iterator socket;
	for (socket = sockets.begin(); socket != sockets.end(); socket++) {
		if (socket->revents == 0) continue;
        std::map<int, AEventHandler*>::iterator entry = registry_.find(socket->fd);
        if (entry == registry_.end()) continue;
        entry->second->handleEvent(socket->revents);
	}

    dispatching_ = false;
    flushRem();
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

void Dispatcher::flushRem()
{
    if (pendingRem_.empty()) return;

    std::vector<int>::iterator socket;
    for (socket = pendingRem_.begin(); socket != pendingRem_.end(); socket++) {
        std::map<int, AEventHandler*>::iterator entry = registry_.find(*socket);
        if (entry == registry_.end()) return;
        delete entry->second;
        registry_.erase(entry);
    }
}
