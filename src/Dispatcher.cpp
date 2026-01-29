/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 02:22:04 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/29 18:29:39 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dispatcher.hpp"
#include "EventHandler.hpp"
#include "IReactor.hpp"
#include <sys/poll.h>
#include <unistd.h>
#include <utility>
#include <vector>

Dispatcher::Dispatcher() : dispatching_(false) {}

Dispatcher::~Dispatcher() {}

void	Dispatcher::addHandler(int socket, AEventHandler* handler)
{
    short interest = handler->interest();
    HandlerEntry handlerEntry = {
        .handler = handler, 
        .cachedEvents = interest
    };

    if (dispatching_) {
        pendingAdd_.insert(std::pair<int, HandlerEntry>(socket, handlerEntry));
        return;
    }

	registry_.insert(std::pair<int, HandlerEntry>(socket, handlerEntry));
    poll_.add(socket, interest);
}

void	Dispatcher::remHandler(int socket)
{
    if (dispatching_) {
        pendingRem_.insert(socket);
        return;
    }

    std::map<int, HandlerEntry>::iterator entry = registry_.find(socket);
    if (entry == registry_.end()) return;
    delete entry->second.handler;
    registry_.erase(entry);
    poll_.remove(socket);
}

void    Dispatcher::updateEvents(int socket)
{
    std::map<int, HandlerEntry>::iterator entry = registry_.find(socket);
    if (entry == registry_.end()) return;
    short interest = entry->second.handler->interest();
    if (entry->second.cachedEvents == interest) return;
    entry->second.cachedEvents = interest;

    if (dispatching_) {
        pendingUpdate_.insert(socket);
        return;
    }
    poll_.setEvents(socket, interest);
}

void	Dispatcher::tick(int timeoutms)
{
    dispatching_ = true;

	poll_.wait(timeoutms);
    std::vector<pollfd> sockets = poll_.results();
    std::vector<pollfd>::iterator socket;
    for (socket = sockets.begin(); socket != sockets.end(); ++socket) {
        if (socket->revents == 0) continue;
        std::map<int, HandlerEntry>::iterator entry = registry_.find(socket->fd);
        if (entry == registry_.end()) continue;
        entry->second.handler->handleEvent(socket->revents);
        updateEvents(socket->fd);
    }

    dispatching_ = false;

    flushUpdate();
    flushAdd();
    flushRem();
}

void Dispatcher::flushAdd()
{
    if (pendingAdd_.empty()) return;

    std::map<int, HandlerEntry>::iterator entry;
    for (entry = pendingAdd_.begin(); entry != pendingAdd_.end(); entry++) {
        short interest = entry->second.handler->interest();
        HandlerEntry handlerEntry = {
            .handler = entry->second.handler,
            .cachedEvents = interest
        };
        registry_.insert(std::pair<int, HandlerEntry>(entry->first, handlerEntry));
        poll_.add(entry->first, interest);
    }
    pendingAdd_.clear();
}

void Dispatcher::flushRem()
{
    if (pendingRem_.empty()) return;

    std::set<int>::iterator socket;
    for (socket = pendingRem_.begin(); socket != pendingRem_.end(); socket++) {
        std::map<int, HandlerEntry>::iterator entry = registry_.find(*socket);
        if (entry == registry_.end()) continue;
        delete entry->second.handler;
        poll_.remove(*socket);
        registry_.erase(entry);
    }
}

void Dispatcher::flushUpdate()
{
    if (pendingUpdate_.empty()) return;

    std::set<int>::iterator socket;
    for (socket = pendingUpdate_.begin(); socket != pendingUpdate_.end(); socket++) {
        std::map<int, HandlerEntry>::iterator entry = registry_.find(*socket);
        if (entry == registry_.end()) continue;
        poll_.setEvents(*socket, entry->second.cachedEvents);
    }
    pendingUpdate_.clear();
}
