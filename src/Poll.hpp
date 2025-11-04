/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poll.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 16:31:34 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/27 18:49:09 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLL_HPP
# define POLL_HPP

#include <sys/poll.h>
#include <vector>

class Poll {
	public:
		Poll();
		~Poll();

		int	wait(std::vector<pollfd>& sockets, int timeout);
};

#endif
