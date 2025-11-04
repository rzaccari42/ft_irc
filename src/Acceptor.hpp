/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Acceptor.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 03:59:26 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/28 03:01:23 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCEPTOR_HPP
# define ACCEPTOR_HPP

#include "EventHandler.hpp"

class Acceptor : public AEventHandler {
	public:
		Acceptor(int socket, IReactor& reactor);
		~Acceptor();

		void handleEvent(short event);

	private:
};

#endif
