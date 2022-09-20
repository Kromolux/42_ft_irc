# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/12 15:48:28 by rkaufman          #+#    #+#              #
#    Updated: 2022/09/20 10:19:38 by rkaufman         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

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

CFLAGS		=	-Wall -Wextra -Werror -std=c++98 -g
DFLAG		:=	-g -D DEBUG=1

PORT		:=	6667
PASS		:=	Wolfsburg

all:
	make -j $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS): %.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

debug: CFLAGS += $(DFLAG)
debug: re
	./$(NAME)

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

.PHONY: clean fclean re
