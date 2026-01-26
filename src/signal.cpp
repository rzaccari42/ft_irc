/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 02:03:19 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/26 02:04:29 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signal.hpp"

volatile sig_atomic_t g_running = 1;

static void handleSigInt(int)
{
    g_running = 0;
}

void setupSignalHandlers()
{
    struct sigaction sa;
    sa.sa_handler = handleSigInt;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
}
