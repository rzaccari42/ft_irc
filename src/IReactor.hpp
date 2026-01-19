/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IReactor.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 02:05:28 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/19 18:19:31 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IREACTOR_HPP
# define IREACTOR_HPP

class AEventHandler;

class IReactor {
	public:
		virtual ~IReactor() {}

		virtual void	addHandler(int socket, AEventHandler* eventHandler) = 0;
		virtual void	remHandler(int socket) = 0;
        virtual void    updateEvents(int socket) = 0;

        virtual void    tick(int timeoutms) = 0;
};

#endif
