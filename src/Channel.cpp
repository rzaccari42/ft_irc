/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 22:32:05 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/24 16:59:23 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Connection.hpp"
#include <iostream>

Channel::Channel(std::string const& name)
: name_(name)
, members_()
, operators_()
, topic_()
, invited_()
, modes_(0)
, key_()
, limit_(0)
{}

Channel::~Channel() {}

std::string const& Channel::name() const { return name_; }

bool Channel::empty() const { return members_.empty(); }

std::size_t Channel::memberCount() const { return members_.size(); }

Channel::Members const& Channel::members() const { return members_; }

Channel::Members const& Channel::operators() const { return operators_; }

bool Channel::addMember(Connection& connection)
{
    bool wasEmpty = members_.empty();

    if (members_.insert(&connection).second == false) return false;
    if (wasEmpty)
        operators_.insert(&connection);
    return true;
}

bool Channel::addOperator(Connection& connection)
{
    if (hasMember(&connection) == false) return false;
    return (operators_.insert(&connection).second);
}

bool Channel::remMember(Connection& connection)
{
    Members::iterator member = members_.find(&connection);
    if (member == members_.end()) return false;

    members_.erase(member);
    remOperator(connection); // test
    return true;
}

bool Channel::remOperator(Connection& connection)
{
    Members::iterator op = operators_.find(&connection);
    if (op == operators_.end()) return false;
    operators_.erase(op);
    return true;
}

bool Channel::hasMember(Connection const* connection) const
{
    if (!connection) return false;
    return members_.find(const_cast<Connection*>(connection)) != members_.end();
}

bool Channel::isOperator(Connection const* connection) const
{
    if (!connection) return false;
    return operators_.find(const_cast<Connection*>(connection)) != operators_.end();
}

bool Channel::isInvitedNick(std::string const& nick) const
{
    return invited_.find(nick) != invited_.end();
}

void Channel::inviteNick(std::string const& nick)
{
    // do checks
    invited_.insert(nick);
}

void Channel::consumeInviteNick(std::string const& nick)
{
    // do checks
    invited_.erase(nick);
}

void Channel::broadcast(Connection& sender, std::string const& message)
{
    for (Members::iterator it = members_.begin(); it != members_.end(); ++it) {
        Connection* dst = *it;
        if (!dst) {
            std::cout << "no dest\n";
            continue;
        }
        if (dst == &sender) continue;
        dst->queueSend(message);
    }
}

bool Channel::hasMode(ModeBits mode) const { return (modes_ & mode) != 0; }

void Channel::setMode(ModeBits mode) { modes_ |= mode; }

void Channel::clearMode(ModeBits mode) { modes_ &= ~mode; }

std::string const& Channel::topic() const { return topic_; }

void Channel::setTopic(const std::string& topic) { topic_ = topic; }

std::string const& Channel::key() const { return key_; }

void Channel::setKey(const std::string& k)
{
    key_ = k;
    setMode(MODE_KEY);
}

void Channel::clearKey()
{
    key_.clear();
    clearMode(MODE_KEY);
}

std::size_t Channel::limit() const { return limit_; }

void Channel::setLimit(std::size_t n)
{
    limit_ = n;
    setMode(MODE_LIMIT);
}

void Channel::clearLimit()
{
    limit_ = 0;
    clearMode(MODE_LIMIT);
}
