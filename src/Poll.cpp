/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poll.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 16:31:44 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/29 02:05:31 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Poll.hpp" 
#include <cstdlib>
#include <vector>

Poll::Poll() {}

Poll::~Poll() {}

int	Poll::wait(std::vector<pollfd>& sockets, int timeout) {
	if (sockets.empty()) return 0;

	int ready = poll(sockets.data(), sockets.size(), timeout);
	if (ready == -1) return -1; // throw exception if critical
	return ready;
}
