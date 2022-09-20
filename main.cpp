/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/12 15:50:55 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/20 22:27:08 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <cctype>
#include <signal.h>
#include "Server.hpp"

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
		std::cout << "SIGINT signal handler called!\n";
	}
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Invalid number of arguments!\n <port> <password>\n";
		return (EXIT_FAILURE);
	}
	if (isInt(argv[1]) == false)
	{
		std::cerr << "Invalid port as first argument!\n 0 - 65535\n";
		return (EXIT_FAILURE);
	}
	int port = std::atoi(argv[1]);
	if (port < 0 || port > 65535)
	{
		std::cerr << "Invalid port number!\n 0 - 65535\n";
		return (EXIT_FAILURE);
	}
	signal(SIGINT, &sigint);
	Server IRC_Server(std::atoi(argv[1]), argv[2]);

	IRC_Server.init_server();

	while (server_run)
		IRC_Server.run_server();

	IRC_Server.stop_server();

	return (0);
}