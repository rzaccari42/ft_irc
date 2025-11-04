/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 03:35:35 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/28 02:54:11 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTHANDLER_HPP
# define EVENTHANDLER_HPP

#include "IReactor.hpp"

class AEventHandler {
	public:
		AEventHandler(int socket, IReactor& reactor);
		virtual ~AEventHandler();

		int	getSocket() const;

		virtual void	handleEvent(short event) = 0;

	protected:
		int			socket_;
		IReactor&	reactor_;
};

#endif
