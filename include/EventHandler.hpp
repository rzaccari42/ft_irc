/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 03:35:35 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/22 18:43:21 by razaccar         ###   ########.fr       */
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

		virtual void	handleEvent(short revent);
        virtual short   interest() = 0;

	protected:
		int			socket_;
		IReactor&	reactor_;

        virtual void    onReadable() = 0;
        virtual void    onWritable() = 0;
        virtual void    onError(short revents) = 0;
};

#endif
