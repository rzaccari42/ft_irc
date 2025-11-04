/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 19:56:22 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/29 03:05:25 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include "EventHandler.hpp"

#define BUFFSIZE 512

class Connection : public AEventHandler {
	public:
		Connection(int socket, IReactor& reactor);
		~Connection();

		void handleEvent(short event);

	private:
		char*	buf_;
};

#endif
