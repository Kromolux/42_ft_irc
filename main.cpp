/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/12 15:50:55 by rkaufman          #+#    #+#             */
/*   Updated: 2022/10/03 16:26:32 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <cctype>
#include <signal.h>

#include "Server.hpp"
#include "colors.h"
#ifndef DEBUG
# define DEBUG 0
#endif

bool server_run = true;

bool	isInt(char *userInput)
{
	for (int i = 1; userInput[i]; ++i)
	{
		if (isdigit(userInput[i]) == false)
			return (false);
	}
	return (true);
}

void	sigint(int sign)
{
	if (sign == SIGINT)
	{
		server_run = false;
		std::cout << COLOR_PURPLE << "SIGINT signal handler called!\n" << COLOR_DEFAULT;
	}
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << COLOR_RED << "Invalid number of arguments!\n <port> <password>\n" << COLOR_DEFAULT;
		return (EXIT_FAILURE);
	}
	if (isInt(argv[1]) == false)
	{
		std::cerr << COLOR_RED << "Invalid port as first argument!\n 0 - 65535\n" << COLOR_DEFAULT;
		return (EXIT_FAILURE);
	}
	int port = std::atoi(argv[1]);
	if (port < 0 || port > 65535)
	{
		std::cerr << COLOR_RED << "Invalid port number!\n 0 - 65535\n" << COLOR_DEFAULT;
		return (EXIT_FAILURE);
	}
	signal(SIGINT, &sigint);
	Server IRC_Server(std::atoi(argv[1]), argv[2]);

	if (IRC_Server.init_server() == EXIT_FAILURE)
		return (EXIT_FAILURE);

	std::cout << COLOR_GREEN << "IRC Server is up and running...\n" << COLOR_DEFAULT;

	while (server_run)
		IRC_Server.run_server();

	IRC_Server.stop_server();

	return (EXIT_SUCCESS);
}
