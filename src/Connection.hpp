/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 19:56:22 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/22 18:44:15 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include "EventHandler.hpp"
#include "Client.hpp"

#define BUFFSIZE 10

class IRCServer;

class Connection : public AEventHandler {
	public:
		Connection(int socket, IReactor& reactor, IRCServer& server);
		~Connection();

        IRCServer&  server();
        Client&     client();
        int         socket();
        short       interest();

        void        queueSend(std::string const& message);

	private:
        IRCServer&  server_;
        Client      client_;
        std::string	in_;
        std::string	out_;

        void    onReadable();
        void    onWritable();
        void    onError(short revents);
};

#endif
