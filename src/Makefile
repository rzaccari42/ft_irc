# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/28 03:29:48 by razaccar          #+#    #+#              #
#    Updated: 2026/01/19 03:21:09 by razaccar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRCS = 	main.cpp IRCServer.cpp Demultiplexer.cpp Listener.cpp Dispatcher.cpp EventHandler.cpp  \
		Acceptor.cpp Connection.cpp Client.cpp Parser.cpp CmdProcessor.cpp

OBJS = ${SRCS:.cpp=.o}

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME) 

re: clean all
