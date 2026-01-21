/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 01:55:58 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/21 12:30:06 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

#include "IReactor.hpp"
#include "Channel.hpp"
#include "CmdProcessor.hpp"
#include "Listener.hpp"
#include <string>
#include <map>

class Connection;

class IRCServer {
    public:
        IRCServer(int port, std::string const& password, IReactor& reactor);
        ~IRCServer();

        CmdProcessor&       protocol();
        std::string const&  password() const;

        void                run();
        void                addConnection(Connection* connection);
        void                onDisconnect(Connection& connection);

        bool                bindNick(int sock, std::string const& nick);
        void                unbindNick(int fd);
        Connection*         findBySock(int sock);
        Connection*         findByNick(std::string const& nick);

        Channel*            findChannel(std::string const& name);
        Channel&            getOrCreateChannel(std::string const& name);
        void                eraseChannelIfEmpty(std::string const& name);

    private:
        // Configuration
        int             port_;
        std::string     password_;
        Listener        listener_;
        IReactor&       reactor_;
        CmdProcessor    protocol_;

        // Global state
        std::map<int, Connection*>      connections_;
        std::map<std::string, int>      nickToSock_;
        std::map<std::string, Channel>  channels_;
        bool                            stop_;
};

#endif
