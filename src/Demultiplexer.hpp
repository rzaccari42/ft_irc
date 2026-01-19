/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Demultiplexer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/17 04:40:20 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/17 05:55:34 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEMULTIPLEXER_HPP
#define DEMULTIPLEXER_HPP

#include <vector>
#include <map>
#include <cstddef> 
#include <poll.h>

class Demultiplexer {
    public:
        Demultiplexer();
        ~Demultiplexer();

        void add(int socket, short events);
        void remove(int socket);
        void setEvents(int socket, short events);

        int wait(int timeout);

        const std::vector<pollfd>& results() const;
        std::vector<pollfd>&       results();

    private:
        std::vector<pollfd>        sockets_;
        std::map<int, std::size_t> socketsIndex_;
};

#endif 

