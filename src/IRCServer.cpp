/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 01:51:31 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/26 03:24:04 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServer.hpp"
#include "Listener.hpp"
#include "Acceptor.hpp"
#include "Connection.hpp"
#include "signal.hpp"
#include <string>
#include <iostream>

IRCServer::IRCServer(int port, const std::string& password, IReactor& reactor)
:   port_(port),
    password_(password),
    listener_(port),
    reactor_(reactor),
    protocol_()
{
    Acceptor* acceptor = new Acceptor(listener_.getSockfd(), reactor_, *this);
    reactor_.addHandler(listener_.getSockfd(), acceptor);
}

IRCServer::~IRCServer()
{
    shutdown();
}

CmdProcessor& IRCServer::protocol() { return protocol_; }

const std::string& IRCServer::password() const { return password_; }

void IRCServer::run()
{
    setupSignalHandlers();
    do { reactor_.tick(100); } while (g_running);
}

void IRCServer::shutdown()
{
    std::cout << "\nShutting down server...\n";
    std::map<int, Connection*>::iterator it = connections_.begin();
    for (; it != connections_.end(); ++it)
        reactor_.remHandler(it->second->socket());
    reactor_.remHandler(listener_.getSockfd());
}

void IRCServer::addConnection(Connection* connection)
{
    if (!connection) return;
    int const sock = connection->getSocket();
    connections_[sock] = connection;
}

void IRCServer::onDisconnect(Connection& connection)
{
    std::cout << "Disconnecting " << connection.client().getNick() << std::endl;
    if (channels_.size() > 0) {
        std::map<std::string, Channel>::iterator channel = channels_.begin();
        for (; channel != channels_.end(); ++channel) {
            if (channel->second.hasMember(&connection))
                channel->second.remMember(connection);
        }
    }
    int const sock = connection.getSocket();
    unbindNick(sock);
    connections_.erase(sock);
    reactor_.remHandler(sock);
}

bool IRCServer::bindNick(int sock, std::string const& nick)
{
    if (nick.empty()) return false;
    std::map<std::string, int>::iterator entry = nickToSock_.find(nick);
    if (entry != nickToSock_.end() && entry->second != sock) return false;
    unbindNick(sock);
    nickToSock_[nick] = sock;
    return true;
}

void IRCServer::unbindNick(int sock)
{
    std::map<std::string, int>::iterator it = nickToSock_.begin();
    for (; it != nickToSock_.end(); it++) {
        if (it->second == sock) {
            nickToSock_.erase(it);
            return;
        }
    }
}

Connection* IRCServer::findBySock(int sock)
{
    std::map<int, Connection*>::iterator c = connections_.find(sock);
    return (c == connections_.end()) ? NULL : c->second;
}

Connection* IRCServer::findByNick(std::string const& nick)
{
    std::map<std::string, int>::iterator entry = nickToSock_.find(nick);
    if (entry == nickToSock_.end()) return 0;
    return findBySock(entry->second);
}

Channel* IRCServer::findChannel(std::string const& name)
{
    std::map<std::string, Channel>::iterator channel = channels_.find(name);
    if (channel == channels_.end()) return 0;
    return &channel->second;
}

Channel& IRCServer::getOrCreateChannel(std::string const& name)
{
    std::map<std::string, Channel>::iterator channel = channels_.find(name);
    if (channel != channels_.end()) return channel->second;

    std::pair<std::map<std::string, Channel>::iterator, bool> ins =
        channels_.insert(std::make_pair(name, Channel(name)));
    return ins.first->second;
}

void IRCServer::eraseChannelIfEmpty(std::string const& name)
{
    std::map<std::string, Channel>::iterator channel = channels_.find(name);
    if (channel != channels_.end() && channel->second.empty())
        channels_.erase(channel);
}
