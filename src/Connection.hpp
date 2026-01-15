/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 19:56:22 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/13 05:48:51 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include "EventHandler.hpp"
#include "Client.hpp"

#define BUFFSIZE 10

class Connection : public AEventHandler {
	public:
		Connection(int socket, IReactor& reactor);
		~Connection();

		void handleEvent(short event);

	private:
        Client      client;
		char*	    buf_;
        std::string message_;
};

#endif
