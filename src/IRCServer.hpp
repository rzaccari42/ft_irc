/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 01:55:58 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/28 15:06:45 by razaccar         ###   ########.fr       */
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

#define SERVER_NAME "irc.42.fr"

enum ReplyCode {
    RPL_WELCOME             = 001,
    RPL_CHANNELMODEIS       = 324,
    RPL_NOTOPIC             = 331,
    RPL_TOPIC               = 332,
    RPL_INVITING            = 341,
    RPL_NAMREPLY            = 353,
    RPL_ENDOFNAMES          = 366,
    ERR_NOSUCHNICK          = 401,
    ERR_NOSUCHCHANNEL       = 403,
    ERR_NOTEXTTOSEND        = 412,
    ERR_ERRONEUSNICKNAME    = 432,
    ERR_NICKNAMEINUSE       = 433,
    ERR_USERNOTINCHANNEL    = 441,
    ERR_NOTONCHANNEL        = 442,
    ERR_USERONCHANNEL       = 443,
    ERR_NOTREGISTERED       = 451,
    ERR_NEEDMOREPARAMS      = 461,
    ERR_ALREADYREGISTRED    = 462,
    ERR_PASSWDMISMATCH      = 464,
    ERR_CHANNELISFULL       = 471,
    ERR_UNKNOWNMODE         = 472,
    ERR_INVITEONLYCHAN      = 473,
    ERR_BADCHANNELKEY       = 475,
    ERR_CHANOPRIVSNEEDED    = 482
};

class Connection;

class IRCServer {
    public:
        IRCServer(int port, std::string const& password, IReactor& reactor);
        ~IRCServer();

        CmdProcessor&       protocol();
        std::string const&  password() const;

        void                run();
        void                shutdown();
        void                addConnection(Connection* connection);
        void                onDisconnect(Connection& connection);

        bool                bindNick(int sock, std::string const& nick);
        void                unbindNick(int sock);
        Connection*         findBySock(int sock);
        Connection*         findByNick(std::string const& nick);

        Channel*            findChannel(std::string const& name);
        Channel&            getOrCreateChannel(std::string const& name);
        bool                eraseChannelIfEmpty(std::string const& name);

    private:
        // Configuration
        std::string     password_;
        Listener        listener_;
        IReactor&       reactor_;
        CmdProcessor    protocol_;

        // Global state
        std::map<int, Connection*>      connections_;
        std::map<std::string, Channel>  channels_;
        std::map<std::string, int>      nickToSock_;
};

#endif
