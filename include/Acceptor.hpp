/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Acceptor.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 03:59:26 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/22 18:45:24 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCEPTOR_HPP
# define ACCEPTOR_HPP

#include "EventHandler.hpp"

class IRCServer;

class Acceptor : public AEventHandler {
	public:
		Acceptor(int socket, IReactor& reactor, IRCServer& server);
		~Acceptor();

        short   interest();

	private:
        IRCServer&  server_;

        void    onReadable();
        void    onWritable();
        void    onError(short revents);
};

#endif
