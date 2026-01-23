/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdProcessor.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 06:35:09 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/23 00:52:14 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CMDPROCESSOR_HPP
# define CMDPROCESSOR_HPP

#include "Parser.hpp"

class Connection;

typedef enum Cmd {
    UNKNOWN,
    PASS,
    NICK,
    USER,
    PRIVMSG,
    QUIT,
    JOIN,
    TOPIC,
    INVITE,
    KICK,
    MODE,
    CAP,
    PING,
    CMD_COUNT
} Cmd;

class CmdProcessor {
    public:
        CmdProcessor();
        ~CmdProcessor();

        void    onMessage(Connection& connection, std::string const& msg);

    private:
        Cmd     getCmd(std::string const& s);
        void    dispatch(Connection& connection, Message const& cmd);
        void    sendError(Connection& connection, std::string const& err);

        typedef void (CmdProcessor::*CmdHandler)(Connection&, Message const&);
        void    handlePass(Connection& connection, Message const& cmd);
        void    handleNick(Connection& connection, Message const& cmd);
        void    handleUser(Connection& connection, Message const& cmd);
        void    handlePrivmsg(Connection& connection, Message const& cmd);
        void    handleQuit(Connection& connection, Message const& cmd);
        void    handleJoin(Connection& connection, Message const& cmd);
        void    handleTopic(Connection& connection, Message const& cmd);
        void    handleInvite(Connection& connection, Message const& cmd);
        void    handleKick(Connection& connection, Message const& cmd);
        void    handleMode(Connection& connection, Message const& cmd);
        void    handleCap(Connection& connection, Message const& cmd);
        void    handlePing(Connection& connection, Message const& cmd);

        static CmdHandler handlers_[CMD_COUNT];
};

#endif
