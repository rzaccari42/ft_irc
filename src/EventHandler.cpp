/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 03:35:58 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/28 22:39:04 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventHandler.hpp"
#include <poll.h>
#include <sys/poll.h>

AEventHandler::AEventHandler(int socket, IReactor& reactor) 
    : socket_(socket), reactor_(reactor) {}

AEventHandler::~AEventHandler() {}

int AEventHandler::getSocket() const { return socket_; }

void AEventHandler::handleEvent(short revents)
{
    if (revents & (POLLERR | POLLHUP | POLLNVAL | POLLRDHUP)) {
        onError(revents);
        return;
    }
    if (revents & POLLIN) onReadable();
    if (revents & POLLOUT) onWritable();
}

void AEventHandler::onReadable() {}
void AEventHandler::onWritable() {}
void AEventHandler::onError(short) {}

