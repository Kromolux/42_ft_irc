# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/12 15:48:28 by rkaufman          #+#    #+#              #
#    Updated: 2022/09/17 14:41:30 by rkaufman         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		:=	ircserv

CC			:=	c++

SRCS		:=	main.cpp \
				Server.cpp \
				Client.cpp \
				Message.cpp \
				Channel.cpp

OBJS		:=	$(SRCS:%.cpp=%.o)

CFLAGS		=	-Wall -Wextra -Werror -std=c++98
DFLAG		:=	-g -D DEBUG=1

PORT		:=	4242
PASS		:=	Wolfsburg

all: $(NAME)

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
