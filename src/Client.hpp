/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 20:36:30 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/20 23:24:51 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>


class Client {
	public:
		Client();
		~Client();

        std::string const&  getNick() const;
        std::string const&  getUser() const;
        std::string const&  getRealname() const;
        // Mode const&         getMode() const;
        void                setNick(std::string nick);
        void                setUser(std::string user);
        void                setRealname(std::string realname);
        // void                setMode();
        
        bool    isRegistered();
        void    tryRegister();
        void    setPassAccepted();


	private:
        std::string nick_;
        std::string user_;
        std::string realname_;
        // Mode        mode_;
        // Channel     channel;

        bool        passAccepted_;
        bool        registered_;
        // std::string server;
};

#endif
