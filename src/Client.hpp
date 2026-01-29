/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 20:36:30 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/29 18:35:37 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>

class Client {
	public:
		Client();
		~Client();

        std::string const&  getHost() const;
        std::string const&  getNick() const;
        std::string const&  getUser() const;
        std::string const&  getRealname() const;
        void                setHost(std::string host);
        void                setNick(std::string nick);
        void                setUser(std::string user);
        void                setRealname(std::string realname);
        
        bool    isRegistered();
        void    tryRegister();
        void    setPassAccepted();

	private:
        bool        passAccepted_;
        bool        registered_;

        std::string host_;
        std::string nick_;
        std::string user_;
        std::string realname_;
};

#endif
