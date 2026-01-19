/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdProcessor.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 06:34:46 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/19 19:27:46 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CmdProcessor.hpp"
#include "Connection.hpp"
#include "IRCServer.hpp"
#include <cctype>
#include <cstdio>
#include <string>

static bool isValidNickChar(char c)
{
    return std::isalnum(static_cast<unsigned char>(c)) ||
           c == '_' || c == '-' || c == '[' || c == ']' || c == '\\';
}

static bool isValidNick(std::string const& nick)
{
    if (nick.empty() || nick.size() > 30) return false;
    if (nick[0] == '#' || nick[0] == ':') return false;
    for (std::string::size_type  i = 0; i < nick.size(); i++)
        if (!isValidNickChar(nick[i])) return false;
    return true;
}

static std::string replyPrefix(Connection& connection)
{
    (void)connection;
    return ":ircserv ";
}

static std::string clientTag(Connection& connection)
{
    if (connection.client().getNick().empty()) return "*";
    return connection.client().getNick();
}

static void reply(Connection& connection, std::string const& lineNoCrlf)
{
    std::string reply = replyPrefix(connection);
    std::string line = lineNoCrlf;
    if (line.size() < 2 || line.substr(line.size() - 2) != "\r\n")
        line += "\r\n";
    reply += line;
    connection.queueSend(reply);
}

static void replyNumeric(Connection& connection,
                        int code,
                        std::string const& middle,
                        std::string const& trailing)
{
    char buf[16];
    std::sprintf(buf, "%03d", code);

    std::string line = replyPrefix(connection);
    line += buf;
    line += " ";
    line += clientTag(connection);
    if (!middle.empty()) {
        line += " ";
        line += middle;
    }
    if (!trailing.empty()) {
        line += " :";
        line += trailing;
    }
    line += "\r\n";
    connection.queueSend(line);
}

/* ************************************************************************* */

CmdProcessor::CmdProcessor() {}

CmdProcessor::~CmdProcessor() {}

void CmdProcessor::onMessage(Connection& connection, std::string const& s)
{
    Message msg;
    std::string err;
    if (!Parser::parseMsg(s, msg, &err)) {
        sendError(connection, "Invalid input:" + err);
        return;
    }
    dispatch(connection, msg);
}

Cmd CmdProcessor::getCmd(std::string const& s)
{
    if (s == "PASS")    return PASS;
    if (s == "NICK")    return NICK;
    if (s == "USER")    return USER;
    if (s == "PRIVMSG") return PRIVMSG;
    if (s == "QUIT")    return QUIT;
    if (s == "JOIN")    return JOIN;
    if (s == "TOPIC")   return TOPIC;
    if (s == "INVITE")  return INVITE;
    if (s == "KICK")    return KICK;
    if (s == "MODE")    return MODE;
    return UNKNOWN;
}

CmdProcessor::CmdHandler CmdProcessor::handlers_[CMD_COUNT] = {
    0,
    &CmdProcessor::handlePass,
    &CmdProcessor::handleNick,
    &CmdProcessor::handleUser,
    &CmdProcessor::handlePrivmsg,
    &CmdProcessor::handleQuit,
    // &CmdProcessor::handleJoin,
    // &CmdProcessor::handleTopic,
    // &CmdProcessor::handleInvite,
    // &CmdProcessor::handleKick,
    // &CmdProcessor::handleMode,
};

void CmdProcessor::dispatch(Connection& connection, Message const& message)
{
    Cmd cmd = getCmd(message.command);
    if (!connection.client().isRegistered() &&
        cmd != PASS && cmd != NICK && cmd != USER && cmd != QUIT) {
        sendError(connection, "client not registered");
        return;
    }
    if (cmd <= UNKNOWN ||cmd >= CMD_COUNT || handlers_[cmd] == 0) {
        sendError(connection, "unknown command");
        return;
    }
    (this->*handlers_[cmd])(connection, message);
}

void CmdProcessor::sendError(Connection& connection, std::string const& err)
{
    // sanitize: never inject raw CR/LF into IRC output
    std::string clean;
    clean.reserve(err.size());
    for (std::string::size_type i = 0; i < err.size(); ++i) {
        char c = err[i];
        clean += (c == '\r' || c == '\n') ? ' ' : c;
    }
    // always send IRC line terminator
    if (clean.size() < 2 || clean.substr(clean.size() - 2) != "\r\n")
        clean += "\r\n";

    connection.queueSend(clean);
}

void CmdProcessor::handlePass(Connection& connection, Message const& cmd)
{
    if (cmd.params.size() < 1) {
        replyNumeric(connection, 462, "PASS", "Not enough parameters");
        return;
    }
    if (connection.client().isRegistered()) {
        replyNumeric(connection, 462, "", "Client already registered");
        return;
    }
    std::string const& provided = cmd.params[0];
    if (provided != connection.server().password()) {
        // ERR_PASSWDMISMATCH (464)
        replyNumeric(connection, 464, "", "Password incorrect");
        return;
    }
    reply(connection, "Password OK, use USER command to terminate registration.");
    connection.client().setPassAccepted();
    connection.client().tryRegister();
}

void CmdProcessor::handleNick(Connection& connection, Message const& cmd)
{
    if (cmd.params.size() < 1) {
        replyNumeric(connection, 461, "NICK", "Not enough parameters");
        return;
    }
    std::string const nick = cmd.params[0];
    if (!isValidNick(nick)) {
        // ERR_ERRONEUSNICKNAME (432)
        replyNumeric(connection, 432, nick, "Erroneous nickname");
        return;
    }

    int const sock = connection.socket();
    if (!connection.client().getNick().empty())
        connection.server().unbindNick(sock); // include IRCServer ?
    if (!connection.server().bindNick(sock, nick)) {
        // Nick already in use -> ERR_NICKNAMEINUSE (433)
        replyNumeric(connection, 433, nick, "Nickname is already in use");
        return;
    }
    std::string const oldNick = connection.client().getNick();
    connection.client().setNick(nick);

    // if user already registered: broadcast nick change
    if (connection.client().isRegistered()) {
        std::string reply = ":" + (oldNick.empty() ? clientTag(connection) : oldNick);
        reply += " NICK :";
        reply += nick;
        reply += "\r\n";
        connection.queueSend(reply);
        // later: should also propagate to channels/other clients
    }
    connection.client().tryRegister();
}

void CmdProcessor::handleUser(Connection& connection, Message const& cmd)
{
    if (cmd.params.size() < 4) {
        replyNumeric(connection, 461, "USER", "Not enough parameters");
        return;
    }
    if (connection.client().isRegistered()) {
        replyNumeric(connection, 462, "", "Client already registered");
        return;
    }

    std::string const& username = cmd.params[0];
    std::string const& realname = cmd.params[3];
    if (username.empty()) {
        replyNumeric(connection, 461, "USER", "Not enough parameters");
        return;
    }
    if (realname.empty()) {
        replyNumeric(connection, 461, "USER", "Not enough parameters");
        return;
    }
    connection.client().setUser(username);
    connection.client().setRealname(realname);
    connection.client().tryRegister();
}

void CmdProcessor::handlePrivmsg(Connection& connection, Message const& cmd)
{
    if (cmd.params.size() < 2) {
        replyNumeric(connection, 461, "USER", "Not enough parameters");
        return;
    }
    std::string const& target = cmd.params[0];
    std::string const& message = cmd.params[1];
    if (message.empty()) {
        // ERR_NOTEXTTOSEND (412)
        replyNumeric(connection, 412, "", "No text to send");
        return;
    }
    // channel targets (#channel) require Channel.hpp -> not implementable with your current uploads
    // if (!target.empty() && target[0] == '#') {
    //     sendNumeric(connection, 482, target, "Channel PRIVMSG not implemented yet");
    //     return;
    // }
    
    Connection* dst = connection.server().findByNick(target);
    if (!dst) {
        // ERR_NOSUCHNICK (401)
        replyNumeric(connection, 401, target, "No such nick");
        return;
    }
    // maybe other errors to handle
    std::string line = ":" + connection.client().getNick();
    line += " PRIVMSG ";
    line += target;
    line += " :";
    line += message;
    line += "\r\n";
    dst->queueSend(line);
}

void CmdProcessor::handleQuit(Connection& connection, Message const& cmd)
{
    std::string reason = "Client Quit";
    if (!cmd.params.empty() && !cmd.params[0].empty())
        reason = cmd.params[0];
    // // Inform the client
    // std::string line = ":" + clientTag(connection) + " QUIT :" + reason + "\r\n";
    // connection.queueSend(line);
    std::string ctx = "QUIT: " + reason;
    connection.server().onDisconnect(connection, ctx);
}

// void CmdProcessor::handleJoin(Connection& connection, Message const& cmd)
// {}
//
// void CmdProcessor::handleTopic(Connection& connection, Message const& cmd)
// {}
//
// void CmdProcessor::handleInvite(Connection& connection, Message const& cmd)
// {}
//
// void CmdProcessor::handleKick(Connection& connection, Message const& cmd)
// {}
//
// void CmdProcessor::handleMode(Connection& connection, Message const& cmd)
// {}
//
