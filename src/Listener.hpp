/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 22:25:10 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/29 01:55:24 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENER_HPP
# define LISTENER_HPP

#include "Errors.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>

#define MAX_CONNECT 10

class Listener {
	public:
		Listener(uint16_t port);
		~Listener();
		
		int	getSockfd() const;

	private:
		int socket_;
};

#endif
