/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Demultiplexer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 16:31:44 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/23 02:12:04 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Demultiplexer.hpp"
#include <cstdlib>

Demultiplexer::Demultiplexer() {}

Demultiplexer::~Demultiplexer() {}

void Demultiplexer::add(int socket, short events)
{
    pollfd sock = {.fd = socket, .events = events, .revents = 0};
    socketsIndex_.insert(std::make_pair(socket, sockets_.size()));
    sockets_.push_back(sock);
}

void Demultiplexer::remove(int socket)
{
    std::map<int, size_t>::iterator it = socketsIndex_.find(socket);
    if (it == socketsIndex_.end()) return;
    size_t idx = it->second;
    size_t last = sockets_.size() - 1;
    if (idx != last) {
        sockets_[idx] = sockets_[last];
        int movedFd = sockets_[idx].fd;
        std::map<int, size_t>::iterator mit = socketsIndex_.find(movedFd);
        mit->second = idx;
    }
    sockets_.pop_back();
    socketsIndex_.erase(it);
}

void Demultiplexer::setEvents(int socket, short events)
{
    size_t index = socketsIndex_[socket];
    sockets_[index].events = events;
}

int	Demultiplexer::wait(int timeout) {
	if (sockets_.empty()) return 0;

    for (size_t i = 0; i < sockets_.size(); ++i) {
        sockets_[i].revents = 0;
    }
	int ready = poll(sockets_.data(), sockets_.size(), timeout);
	if (ready == -1) return -1; // throw exception if critical
	return ready;
}

const std::vector<pollfd>& Demultiplexer::results() const { return sockets_; }

std::vector<pollfd>& Demultiplexer::results() { return sockets_; }
