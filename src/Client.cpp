/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 20:36:15 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/19 21:40:20 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : passAccepted_(false), registered_(false) {}

Client::~Client() {}

std::string const& Client::getNick() const { return nick_; }

std::string const& Client::getUser() const { return user_; }

std::string const& Client::getRealname() const { return realname_; }

void Client::setNick(std::string nick) { nick_ = nick; }

void Client::setUser(std::string user) { user_ = user; }

void Client::setRealname(std::string realname) { realname_ = realname; }


bool Client::isRegistered() { return registered_; }

void Client::tryRegister()
{
    if (registered_) return;
    if (passAccepted_ == false) return;
    if (nick_.empty()) return;
    if (user_.empty()) return;

    registered_ = true;
}

void Client::setPassAccepted() { passAccepted_ = true; }
