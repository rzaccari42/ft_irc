/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 19:56:37 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/29 07:02:27 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "IReactor.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>

Connection::Connection(int socket, IReactor& reactor)
	: AEventHandler(socket, reactor) {
	buf_ = (char*)malloc(BUFFSIZE * sizeof(char));
	memset(buf_, 0, BUFFSIZE);
}

Connection::~Connection() {
	free(buf_);
}

void	Connection::handleEvent(short event) {
	(void)event;
	recv(socket_, buf_, BUFFSIZE, 0);
	std::cout << buf_ << std::endl;
	// send(socket_, buf_, BUFFSIZE, 0); // echo server test
	memset(buf_, 0, BUFFSIZE);
}
