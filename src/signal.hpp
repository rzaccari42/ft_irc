/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 03:17:16 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/26 02:06:22 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_HPP
# define SIGNAL_HPP  

#include <signal.h>

extern volatile sig_atomic_t g_running;

void setupSignalHandlers();

#endif

// #include <stdexcept>
// #include <string>

// class Error : public std::runtime_error {
// 	public:
// 		explicit Error(std::string const& msg)
// 			: std::runtime_error(msg) {}
// };
//
// class SocketError : public std::runtime_error {
// 	public:
// 		explicit SocketError(std::string const& msg)
// 			: std::runtime_error(msg) {}
// };
