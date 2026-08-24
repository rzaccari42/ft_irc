/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 02:21:48 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/19 18:20:09 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DISPATCHER_HPP
# define DISPATCHER_HPP

#include "IReactor.hpp"
#include "EventHandler.hpp"
#include "Demultiplexer.hpp"
#include <map>
#include <set>

typedef struct HandlerEntry {
    AEventHandler*  handler;
    short           cachedEvents;
} HandlerEntry;

class Dispatcher : public IReactor {
	public: 
		Dispatcher();
		~Dispatcher();
		
		void    addHandler(int socket, AEventHandler* handler);
		void    remHandler(int socket);
        void    updateEvents(int socket);

		void    tick(int timeoutms);

	private:
        Demultiplexer               poll_;
		std::map<int, HandlerEntry> registry_;
		std::map<int, HandlerEntry> pendingAdd_;
		std::set<int>       	    pendingRem_;
        std::set<int>               pendingUpdate_;
        bool                        dispatching_;

        void    flushAdd();
        void    flushRem();
        void    flushUpdate();
};

#endif
