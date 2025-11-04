/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 23:16:58 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/29 01:55:58 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"

Listener::Listener(uint16_t port) {
	sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");

	if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw SocketError("socket syscall failed");
	if (fcntl(socket_,  F_SETFL, O_NONBLOCK) == -1)
		throw SocketError("fcntl syscall failed");
	if (bind(socket_, (sockaddr*)&address, sizeof(sockaddr)) == -1)
		throw SocketError("bind syscall failed");
	if (listen(socket_, MAX_CONNECT) == -1)
		throw SocketError("listen syscall failed");
}

Listener::~Listener() {
	close(socket_);
}

int	Listener::getSockfd() const {
	return socket_;
}
