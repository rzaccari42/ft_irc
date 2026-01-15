/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 02:21:48 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/15 12:52:10 by razaccar         ###   ########.fr       */
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
		
        // redundant socket param -> handler already stores socket
		void addHandler(int socket, AEventHandler* handler);
		void remHandler(int socket);

		void eventLoop();

	private:
        bool                            dispatching_;
		std::map<int, AEventHandler*>	registry_;
		std::vector<int>            	pendingAdd_;
		std::vector<int>            	pendingRem_;

		std::vector<pollfd> getSockets();
        void                flushRem();
};

#endif
