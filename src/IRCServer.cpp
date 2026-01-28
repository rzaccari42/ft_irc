/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 01:51:31 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/28 15:07:06 by razaccar         ###   ########.fr       */
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
:   password_(password),
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
        std::vector<std::string> chans;
        chans.reserve(channels_.size());
        std::map<std::string, Channel>::iterator channel = channels_.begin();
        for (; channel != channels_.end(); ++channel) {
            if (channel->second.hasMember(&connection))
                chans.push_back(channel->first);
        }

        for (size_t i = 0; i < chans.size(); ++i) {
            std::map<std::string, Channel>::iterator it = channels_.find(chans[i]);
            if (it == channels_.end())
                continue;
            std::string prefix = ":";
            prefix += connection.client().getNick();
            prefix += "!";
            prefix += connection.client().getUser();
            prefix += "@";
            prefix += connection.client().getHost();

            Channel& channel = it->second;

            std::string line = prefix + " QUIT :Client Quit\r\n";
            Channel::Members::const_iterator member = channel.members().begin();
            for (; member != channel.members().end(); ++member) {
                if (&connection == *member) continue;
                (*member)->queueSend(line);
            }

            channel.remMember(connection);
            if (connection.server().eraseChannelIfEmpty(chans[i])) continue;
            if (channel.ensureOperator()) {
                Channel::Members::iterator first = channel.members().begin();
                std::string line = ":";
                line += SERVER_NAME;
                line += " MODE " + chans[i];
                line += " +o " + (*first)->client().getNick();
                line += "\r\n";
                Channel::Members::const_iterator member = channel.members().begin();
                for (; member != channel.members().end(); ++member)
                    (*member)->queueSend(line);
            }
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

bool IRCServer::eraseChannelIfEmpty(std::string const& name)
{
    std::map<std::string, Channel>::iterator channel = channels_.find(name);
    if (channel != channels_.end() && channel->second.empty()) {
        channels_.erase(channel);
        return true;
    }
    return false;
}
