/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 19:56:37 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/25 03:53:40 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "IReactor.hpp"
#include "IRCServer.hpp"
#include <cerrno>
#include <ostream>
#include <sys/poll.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <sys/socket.h>

Connection::Connection(int socket, IReactor& reactor, IRCServer& server)
	: AEventHandler(socket, reactor)
    , server_(server) {}

Connection::~Connection() {}

IRCServer& Connection::server() { return server_; }

Client& Connection::client() { return client_; }

int Connection::socket() { return socket_; }

short Connection::interest()
{
    short event = POLLIN;
    if (!out_.empty()) event |= POLLOUT;
    return event;
}

void Connection::queueSend(std::string const& message)
{
    bool wasEmpty = out_.empty();
    out_ += message;
    if (wasEmpty)
        reactor_.updateEvents(socket_);
}

void Connection::onReadable()
{
    char buf[BUFFSIZE];

    while (1) {
        const ssize_t nbytes = recv(socket_, buf, sizeof(buf), 0);
        if (nbytes > 0) {
            in_.append(buf, nbytes);
            continue;
        }
        if (nbytes == 0) {
            server_.onDisconnect(*this);
            return;
        }
        if (errno == EINTR) {
            std::cout << "EINTR\n";
            continue;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            std::cout << "EAGAIN | EWOULDBLOCK\n";
            break;
        }
        server_.onDisconnect(*this);
        return;
    }
    while (1) {
        std::string::size_type nl = in_.find('\n');
        if (nl == std::string::npos) break;
        std::string msg = in_.substr(0, nl + 1);
        std::cout << msg << std::endl;
        in_.erase(0, nl + 1);
        while (!msg.empty() && (msg[msg.size()-1] == '\n' || 
                                msg[msg.size()-1] == '\r')) {
            msg.erase(msg.size()-1);
        }
        if (!msg.empty())
            server_.protocol().onMessage(*this, msg);
    }
}

void Connection::onWritable()
{
    while (!out_.empty()) {
        const ssize_t nbytes = send(socket_, out_.data(), out_.size(), 0);
        if (nbytes > 0) {
            out_.erase(0, nbytes);
            continue;
        }
        if (nbytes < 0 && errno == EINTR) continue;
        if (nbytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) return;
        server_.onDisconnect(*this);
        return;
    }
    reactor_.updateEvents(socket_);
}

void Connection::onError(short revents)
{
    if (revents & POLLNVAL) std::cout << "ERROR (POLLNVAL): invalid socket fd" << std::endl;
    else if (revents & POLLERR) std::cout << "ERROR (POLLERR): ";
    else if (revents & POLLHUP) std::cout << "ERROR (POLLHUP): ";
    std::cerr << "lost connection to socket " << socket_ << "\n";
    server_.onDisconnect(*this);
}
