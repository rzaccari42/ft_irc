/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 03:43:09 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/21 13:30:24 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

class Connection;

class Channel {

    public:
        typedef std::set<Connection*> Members;

        enum ModeBits {
            MODE_INVITE_ONLY = 1 << 0,
            MODE_TOPIC_OP    = 1 << 1,
            MODE_KEY         = 1 << 2,
            MODE_LIMIT       = 1 << 3
        };

        explicit Channel(std::string const& name);
        ~Channel();

        std::string const&  name() const;

        bool                empty() const;
        std::size_t         memberCount() const;

        Members const&      members() const;
        Members const&      operators() const;

        bool                addMember(Connection& c);
        bool                addOperator(Connection& c);
        bool                remMember(Connection& c);
        bool                remOperator(Connection& c); // must remain member
        bool                hasMember(Connection const* c) const;
        bool                isOperator(Connection const* c) const;

        bool                isInvitedNick(std::string const& nick) const;
        void                inviteNick(std::string const& nick);
        void                consumeInviteNick(std::string const& nick); // remove after join

        void                broadcast(Connection& sender, std::string const& message);

        bool                hasMode(ModeBits m) const;
        void                setMode(ModeBits m);
        void                clearMode(ModeBits m);

        std::string const&  topic() const;
        void                setTopic(std::string const& topic);

        std::string const&  key() const;
        void                setKey(std::string const& key);
        void                clearKey();
        std::size_t         limit() const;
        void                setLimit(std::size_t n);
        void                clearLimit();

    private:
        std::string             name_;
        Members                 members_;
        Members                 operators_;
        std::string             topic_;
        std::set<std::string>   invited_;
        unsigned int            modes_;
        std::string             key_;
        size_t                  limit_;
};

#endif
