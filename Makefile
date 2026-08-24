# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/28 03:29:48 by razaccar          #+#    #+#              #
#    Updated: 2026/01/26 02:23:47 by razaccar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

BIN_DIR = bin
INC_DIR = include
SRC_DIR = src
BUILD_DIR = $(SRC_DIR)/build

TARGET = $(BIN_DIR)/$(NAME)

SRCS = 	main.cpp IRCServer.cpp Demultiplexer.cpp Listener.cpp Dispatcher.cpp EventHandler.cpp  \
		Acceptor.cpp Connection.cpp Client.cpp Channel.cpp Parser.cpp CmdProcessor.cpp signal.cpp

OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -I$(INC_DIR)

.PHONY: all clean fclean re

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

clean:
	$(RM) -r $(BUILD_DIR)

fclean: clean
	$(RM) $(TARGET)

re: fclean all
