# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/12 15:48:28 by rkaufman          #+#    #+#              #
#    Updated: 2022/09/29 18:32:53 by rkaufman         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

UNAME		:= $(shell uname)

NAME		:=	ircserv

CC			:=	c++

SRCS		:=	main.cpp \
				Server.cpp \
				Client.cpp \
				Message.cpp \
				Channel.cpp \
				Server_cmds.cpp \
				Server_collect.cpp \
				Server_process.cpp \
				Server_distribute.cpp

OBJS		:=	$(SRCS:%.cpp=%.o)

CFLAGS		=	-Wall -Wextra -Werror -std=c++98
DFLAG		:=	-g -D DEBUG=1

PORT		:=	4242

PASS		:=	Wolfsburg

all:	$(NAME)
#	make -j	$(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS): %.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

debug: CFLAGS += $(DFLAG)
debug: re
	./$(NAME) $(PORT) $(PASS)

clean:
	rm -fr $(OBJS)

fclean: clean
	rm -fr $(NAME)

re: fclean all

val:
	valgrind --leak-check=full -s ./$(NAME) $(PORT) $(PASS)

leak:
	leaks --atExit -- ./$(NAME) $(PORT) $(PASS)

start:
	./$(NAME) $(PORT) $(PASS)

.PHONY: clean fclean re val leak debug start
