/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdProcessor.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 06:34:46 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/21 17:59:06 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CmdProcessor.hpp"
#include "Channel.hpp"
#include "Connection.hpp"
#include "IRCServer.hpp"
#include <cctype>
#include <cstddef>
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

static std::vector<std::string> splitComma(std::string const& s)
{
    std::vector<std::string> out;
    std::string cur;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == ',') {
            out.push_back(cur);
            cur.clear();
        }
        else cur += s[i];
    }
    out.push_back(cur);
    return out;
}

static bool isChannelName(std::string const& s)
{
    return !s.empty() && s[0] == '#';
}

static bool parseSize(std::string const& s, std::size_t& out)
{
    if (s.empty()) return false;
    std::size_t n = 0;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] < '0' || s[i] > '9') return false;
        n = n * 10 + (s[i] - '0');
    }
    out = n;
    return true;
}

static std::string buildNamesList(Channel& channel)
{
    std::string names;
    Channel::Members const& members = channel.members();
    for (Channel::Members::const_iterator member = members.begin(); member != members.end(); member++) {
        Connection* connection = *member;
        if (!connection) continue;
        if (!names.empty()) names += " ";
        if (channel.isOperator(connection)) names += "@";
        names += connection->client().getNick();
    }
    return names;
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
    &CmdProcessor::handleJoin,
    &CmdProcessor::handleTopic,
    &CmdProcessor::handleInvite,
    &CmdProcessor::handleKick,
    &CmdProcessor::handleMode,
};

void CmdProcessor::dispatch(Connection& connection, Message const& message)
{
    Cmd cmd = getCmd(message.command);
    if (cmd <= UNKNOWN ||cmd >= CMD_COUNT || handlers_[cmd] == 0) {
        sendError(connection, "unknown command");
        return;
    }
    if (!connection.client().isRegistered() &&
        cmd != PASS && cmd != NICK && cmd != USER && cmd != QUIT) {
        sendError(connection, "client not registered");
        return;
    }
    (this->*handlers_[cmd])(connection, message);
}

void CmdProcessor::sendError(Connection& connection, std::string const& err)
{
    std::string clean;
    clean.reserve(err.size());
    for (std::string::size_type i = 0; i < err.size(); ++i) {
        char c = err[i];
        clean += (c == '\r' || c == '\n') ? ' ' : c;
    }
    if (clean.size() < 2 || clean.substr(clean.size() - 2) != "\r\n")
        clean += "\r\n";

    std::string errmsg = replyPrefix(connection);
    errmsg += " error: ";
    errmsg += clean;
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
    std::string line = ":" + clientTag(connection) + " QUIT :" + reason + "\r\n";
    connection.queueSend(line);
    connection.server().onDisconnect(connection); // no time to receive queued reply ?
}

void CmdProcessor::handleJoin(Connection& connection, Message const& cmd)
{
    if (cmd.params.size() < 1) {
        replyNumeric(connection, 461, "JOIN", "Not enough parameters");
        return;
    }
    std::vector<std::string> chans = splitComma(cmd.params[0]);
    std::vector<std::string> keys;
    if (cmd.params.size() >= 2) keys = splitComma(cmd.params[1]);

    for (std::size_t i = 0; i < chans.size(); ++i) {
        std::string const& chanName = chans[i];
        std::string key = (i < keys.size()) ? keys[i] : ""; //?
        if (isChannelName(chanName) == false) {
            replyNumeric(connection, 403, chanName, "No such channel");
            continue;
        }

        Channel& channel = connection.server().getOrCreateChannel(chanName);
        if (channel.hasMember(&connection)) {
            replyNumeric(connection, 443, connection.client().getNick() + " " 
                         + chanName, "is already on channel");
            continue;
        }
        if (channel.hasMode(Channel::MODE_INVITE_ONLY) &&
            !channel.isInvitedNick(connection.client().getNick())) {
            replyNumeric(connection, 473, chanName, "Cannot join channel (+i)");
            continue;
        }
        if (channel.hasMode(Channel::MODE_KEY) && key != channel.key()) {
            replyNumeric(connection, 475, chanName, "Cannot join channel (+k)");
            continue;
        }
        if (channel.hasMode(Channel::MODE_LIMIT) && channel.memberCount() >= channel.limit()) {
            replyNumeric(connection, 471, chanName, "Cannot join channel (+l)");
            continue;
        }
        if (channel.hasMode(Channel::MODE_INVITE_ONLY))
            channel.consumeInviteNick(connection.client().getNick());

        channel.addMember(connection);
        std::string joinLine = ":" + connection.client().getNick() 
                             + " JOIN " + chanName + "\r\n";
        Channel::Members::const_iterator member;
        for (member = channel.members().begin(); member != channel.members().end(); ++member)
            (*member)->queueSend(joinLine);
        if (channel.topic().empty())
            replyNumeric(connection, 331, chanName, "No topic is set");
        else
            replyNumeric(connection, 332, chanName, channel.topic());
        std::string names = buildNamesList(channel);
        replyNumeric(connection, 353, "= " + chanName + " " + names, "");
        replyNumeric(connection, 366, chanName, "End of /NAMES list");
    }
}

void CmdProcessor::handleTopic(Connection& connection, Message const& cmd)
{
    if (cmd.params.size() < 1) {
        replyNumeric(connection, 461, "TOPIC", "Not enough parameters");
        return;
    }
    std::string const& chanName = cmd.params[0];
    Channel* channel = connection.server().findChannel(chanName);
    if  (!channel) {
        replyNumeric(connection, 403, chanName, "No such channel");
        return;
    }

    if (cmd.params.size() == 1) {
        if (channel->topic().empty())
            replyNumeric(connection, 331, chanName, "No topic is set");
        else
            replyNumeric(connection, 332, chanName, channel->topic());
        return;
    }
    
    if (channel->hasMember(&connection) == false) {
        replyNumeric(connection, 442, chanName, "You're not on that channel");
        return;
    } // ??
    if (channel->hasMode(Channel::MODE_TOPIC_OP) && channel->isOperator(&connection) == false) {
        replyNumeric(connection, 482, chanName, "You're not channel operator");
        return;
    }
    std::string const& newTopic = cmd.params[1];
    channel->setTopic(newTopic);

    std::string line = ":" + connection.client().getNick() + " TOPIC " + chanName + " :" + newTopic + "\r\n";
    Channel::Members::const_iterator member;
    for (member = channel->members().begin(); member != channel->members().end(); ++member)
        (*member)->queueSend(line);
}

void CmdProcessor::handleInvite(Connection& connection, Message const& cmd)
{
    if (cmd.params.size() < 2) {
        replyNumeric(connection, 461, "INVITE", "Not enough parameters");
        return;
    }
    
    std::string const& targetNick = cmd.params[0];
    std::string const& chanName = cmd.params[2];
    
    Channel* channel = connection.server().findChannel(chanName);
    if (!channel) {
        replyNumeric(connection, 403, chanName, "No such channel");
        return;
    }
    // if channel exists only member of the channel are allowed to invite other users
    if (channel->hasMember(&connection) == false) {
        replyNumeric(connection, 442, chanName, "You're not on that channel");
        return;
    }
    // if MODE_INVITE_ONLY -> only operator can issue INVITE cmd
    if (channel->hasMode(Channel::MODE_INVITE_ONLY) && channel->isOperator(&connection)) {
        replyNumeric(connection, 482, chanName, "You're not channel operator");
        return;
    }

    Connection* target = connection.server().findByNick(targetNick);
    if (!target) {
        replyNumeric(connection, 401, targetNick, "No such nick");
        return;
    }
    if (channel->hasMember(target)) {
        replyNumeric(connection, 443, targetNick + " " + chanName, "is already on channel");
        return;
    }

    channel->inviteNick(targetNick);

    // RPL_INVITING (341)
    replyNumeric(connection, 341, targetNick + " " + chanName, "");
    std::string line = ":" + connection.client().getNick() + " INVITE "
                     + targetNick + " :" + chanName + "\r\n";
    target->queueSend(line);
}

void CmdProcessor::handleKick(Connection& connection, Message const& cmd)
{
    if (cmd.params.size() < 2) {
        replyNumeric(connection, 461, "KICK", "Not enough parameters");
        return;
    }

    std::string const& chanName = cmd.params[0];
    std::string const& targetNick = cmd.params[1];
    std::string reason = (cmd.params.size() >= 3) ? cmd.params[2] : "Kicked";

    Channel* channel = connection.server().findChannel(chanName);
    if (!channel) {
        replyNumeric(connection, 403, chanName, "No such channel");
        return;
    }
    if (!channel->hasMember(&connection)) {
        replyNumeric(connection, 442, chanName, "You're not on that channel");
        return;
    }
    if (!channel->isOperator(&connection)) {
        replyNumeric(connection, 482, chanName, "You're not channel operator");
        return;
    }

    Connection* target = connection.server().findByNick(targetNick);
    if (!target) {
        replyNumeric(connection, 401, targetNick, "No such nick");
        return;
    }
    if (!channel->hasMember(target)) {
        replyNumeric(connection, 441, targetNick + " " + chanName, "They aren't on that channel");
        return;
    }

    channel->remMember(*target);
    connection.server().eraseChannelIfEmpty(chanName);

    std::string line = ":" + connection.client().getNick()
                     + " KICK " + chanName + " " + targetNick
                     + " :" + reason + "\r\n";
    Channel::Members::const_iterator member = channel->members().begin();
    for (; member != channel->members().end(); ++member)
        (*member)->queueSend(line);
    target->queueSend(line);
}

void CmdProcessor::handleMode(Connection& connection, Message const& cmd)
{
    if (cmd.params.size() < 1) {
        replyNumeric(connection, 461, "MODE", "Not enough parameters");
        return;
    }
    std::string const& chanName = cmd.params[0];
    Channel* channel = connection.server().findChannel(chanName);
    if (!channel) {
        replyNumeric(connection, 403, chanName, "No such channel");
        return;
    }

    if (cmd.params.size() == 1) {
        std::string modes = "+";
        if (channel->hasMode(Channel::MODE_INVITE_ONLY)) modes += "i";
        if (channel->hasMode(Channel::MODE_TOPIC_OP))    modes += "t";
        if (channel->hasMode(Channel::MODE_KEY))         modes += "k";
        if (channel->hasMode(Channel::MODE_LIMIT))       modes += "l";

        std::string params;
        if (channel->hasMode(Channel::MODE_KEY))   params += " " + channel->key();
        if (channel->hasMode(Channel::MODE_LIMIT)) {
            char buf[32];
            std::sprintf(buf, "%lu", (unsigned long)channel->limit());
            params += " ";
            params += buf;
        }
        // RPL_CHANNELMODEIS (324) with a simple +modes listing
        replyNumeric(connection, 324, chanName + " " + modes + params, "");
        return;
    }
    
    if (!channel->hasMember(&connection)) {
        replyNumeric(connection, 442, chanName, "You're not on that channel");
        return;
    }
    if (!channel->isOperator(&connection)) {
        replyNumeric(connection, 482, chanName, "You're not channel operator");
        return;
    }
    std::string modeStr = cmd.params[1];
    std::size_t argi = 2;
    char sign = 0;
    std::string appliedModes;
    std::vector<std::string> appliedArgs;
    for (std::size_t i = 0; i < modeStr.size(); ++i) {
        char m = modeStr[i];
        if (m == '+' || m == '-') { sign = m; }
        if (!sign) {
            std::string bad;
            bad += m;
            replyNumeric(connection, 472, bad, "is unknown mode char to me");
            return;
            //check if correct
        }
        bool add = (sign == '+');
        if (m == 'i') {
            if (add) channel->setMode(Channel::MODE_INVITE_ONLY);
            else     channel->clearMode(Channel::MODE_INVITE_ONLY);
            appliedModes += (add ? "+i" : "-i");
        }
        else if (m == 't') {
            if (add) channel->setMode(Channel::MODE_TOPIC_OP);
            else     channel->clearMode(Channel::MODE_TOPIC_OP);
            appliedModes += (add ? "+t" : "-t");
        }
        else if (m == 'k') {
            if (add) {
                if (argi >= cmd.params.size()) {
                    replyNumeric(connection, 461, "MODE", "Not enough parameters");
                    return;
                }
                channel->setKey(cmd.params[argi++]);
                appliedModes += "+k";
                appliedArgs.push_back(channel->key());
            }
            else {
                channel->clearKey();
                appliedModes += "-k";
            }
        }
        else if (m == 'l') {
            if (add) {
                if (argi >= cmd.params.size()) {
                    replyNumeric(connection, 461, "MODE", "Not enough parameters");
                    return;
                }
                std::size_t limit;
                if (!parseSize(cmd.params[argi], limit)) {
                    replyNumeric(connection, 461, "MODE", "Invalid limit");
                    return;
                }
                ++argi;
                channel->setLimit(limit);
                appliedModes += "+l";
                char buf[32];
                std::sprintf(buf, "%lu", (unsigned long)limit);
                appliedArgs.push_back(buf);
            }
            else {
                channel->clearLimit();
                appliedModes += "-l";
            }
        }
        else if (m == 'o') {
            if (argi >= cmd.params.size()) {
                replyNumeric(connection, 461, "MODE", "Not enough parameters");
                return;
            }

            std::string targetNick = cmd.params[argi++];
            Connection* target = connection.server().findByNick(targetNick);
            if (!target) {
                replyNumeric(connection, 401, targetNick, "No such nick");
                return;
            }
            if (!channel->hasMember(target)) {
                replyNumeric(connection, 441, targetNick + " " + chanName, "They aren't on that channel");
                return;
            }
            if (add) channel->addOperator(*target);
            else     channel->remOperator(*target);

            appliedModes += (add ? "+o" : "-o");
            appliedArgs.push_back(targetNick);
        }
        else {
            // ERR_UNKNOWNMODE (472)
            std::string t;
            t += m;
            replyNumeric(connection, 472, t, "is unknown mode char to me");
            return;
        }
    }

    if (appliedModes.empty())
        return; // ??

    std::string line = ":" + connection.client().getNick() + " MODE " + chanName + " " + appliedModes;
    for (std::size_t k = 0; k < appliedArgs.size(); ++k) {
        line += " ";
        line += appliedArgs[k];
    }
    line += "\r\n";
    Channel::Members::const_iterator member;
    for (member = channel->members().begin(); member != channel->members().end(); ++member)
        (*member)->queueSend(line);
}
