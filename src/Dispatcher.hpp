/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 02:21:48 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/28 19:26:03 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DISPATCHER_HPP
# define DISPATCHER_HPP

#include "IReactor.hpp"
#include "EventHandler.hpp"
#include <map>
#include <sys/poll.h>
#include <vector>

class Dispatcher : public IReactor {
	public:
		Dispatcher();
		~Dispatcher();
		
		void addHandler(int socket, AEventHandler* handler);
		void remHandler(int socket);

		void eventLoop();

	private:
		std::map<int, AEventHandler*>	registry_;

		std::vector<pollfd>	getSockets();
};

#endif
