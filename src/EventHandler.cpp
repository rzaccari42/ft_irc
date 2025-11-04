/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 03:35:58 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/28 02:58:00 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventHandler.hpp"

AEventHandler::AEventHandler(int socket, IReactor& reactor) : socket_(socket), reactor_(reactor) {}

AEventHandler::~AEventHandler() {}

int AEventHandler::getSocket() const {
	return socket_;
}
