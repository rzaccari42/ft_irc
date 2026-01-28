/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Acceptor.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 04:00:01 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/28 05:03:50 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServer.hpp"
#include "Acceptor.hpp"
#include "Connection.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <new>
#include <sys/poll.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Acceptor::Acceptor(int socket, IReactor& reactor, IRCServer& server)
	: AEventHandler(socket, reactor)
    , server_(server) {}

Acceptor::~Acceptor() {}

short Acceptor::interest() { return POLLIN; }

void Acceptor::onReadable()
{
    while (1) {
        sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int remote = accept(socket_, (sockaddr*)&addr, &len);
        if (remote >= 0) {
            fcntl(remote, F_SETFL, fcntl(remote, F_GETFL) | O_NONBLOCK);
            try {
                Connection* connection = new Connection(remote, reactor_, server_);
                char ip[INET_ADDRSTRLEN];
                connection->client().setHost(inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN));
                reactor_.addHandler(remote, connection);
                server_.addConnection(connection);
            } catch (std::bad_alloc& e) {
                close(remote);
                std::cerr << "ERROR: cannot allocate memory for new connection.\n";
                std::cerr << e.what() << std::endl;
            }
            continue;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
        else {
            std::cerr << "ERROR: accept() failed." << std::endl; // use errno ?
            break;
        }
            
    }
}

void Acceptor::onWritable() {}

void Acceptor::onError(short revents)
{
    if (revents & POLLNVAL) std::cout << "ERROR (POLLNVAL): invalid socket fd" << std::endl;
    else if (revents & POLLERR) 
        std::cout << "ERROR (POLLERR): socket connection failed" << std::endl;
    else if (revents & POLLHUP)
        std::cout << "ERROR (POLLHUP): socket connection failed" << std::endl;
    reactor_.remHandler(socket_); // to check
}
