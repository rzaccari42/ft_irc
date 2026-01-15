/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 19:56:37 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/14 10:49:37 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "IReactor.hpp"
#include <ostream>
#include <sys/poll.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>

Connection::Connection(int socket, IReactor& reactor)
	: AEventHandler(socket, reactor)
{
	buf_ = (char*)malloc(BUFFSIZE * sizeof(char));
	memset(buf_, 0, BUFFSIZE);
}

Connection::~Connection()
{
	free(buf_);
}

void	Connection::handleEvent(short event)
{
    if (event == POLLIN) {
        std::cout << "POLLIN" << std::endl;
        recv(socket_, buf_, BUFFSIZE, 0);
        message_.append(buf_);
        if (strstr(buf_, "\n")) {
            std::cout << message_;
            message_.clear();
        }
        memset(buf_, 0, BUFFSIZE);
    }
    else if (event == POLLOUT) {
        std::cout << "POLLOUT" << std::endl;
        send(socket_, "server message", BUFFSIZE, 0);
    }
    else if (event == POLLPRI) {
        std::cout << "POLLPRI" << std::endl;
    }
    else if (event == POLLERR) {
        std::cout << "POLLERR" << std::endl;
    }
}
