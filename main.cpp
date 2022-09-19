/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/12 15:50:55 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/19 08:39:48 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//socket
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
		std::cout << "Invalid number of arguments!\n <port> <password>\n";
		exit(-1);
	}
	if (isInt(argv[1]) == false)
	{
		std::cout << "Invalid port as first argument!\n 1024 - 65535\n";
		exit(-1);
	}
	signal(SIGINT, &sigint);
	Server IRC_Server(std::atoi(argv[1]), argv[2]);
	IRC_Server.init_server();
	while (server_run)
		IRC_Server.run_server();
	IRC_Server.stop_server();

	return (0);
}