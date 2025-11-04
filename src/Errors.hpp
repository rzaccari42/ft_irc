/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 03:17:16 by razaccar          #+#    #+#             */
/*   Updated: 2025/10/23 20:16:39 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_HPP
# define ERRORS_HPP  

#include <stdexcept>
#include <string>

class Error : public std::runtime_error {
	public:
		explicit Error(std::string const& msg)
			: std::runtime_error(msg) {}
};

class SocketError : public std::runtime_error {
	public:
		explicit SocketError(std::string const& msg)
			: std::runtime_error(msg) {}
};

#endif
