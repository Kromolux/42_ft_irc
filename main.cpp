/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/12 15:50:55 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/16 14:21:30 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//socket
#include <cstdlib>
#include <cctype>

bool	isInt(char *userInput)
{
	for (int i = 1; userInput[i]; ++i)
	{
		if (isdigit(userInput[i]) == false)
			return (false);
	}
	return (true);
}

#include "Server.hpp"

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

	Server IRC_Server(std::atoi(argv[1]), argv[2]);
	IRC_Server.init_server();
	IRC_Server.start_server();
	//struct addrinfo hints, result;
	// struct sockaddr_in serverAddress;
	// serverAddress.sin_family = AF_INET;
	// serverAddress.sin_addr.s_addr = INADDR_ANY;
	// serverAddress.sin_port = htons(atoi(argv[1]));
	//serverAddress.sin_zero = 
	
	//std::vector<struct pollfd>	clients;
	// struct pollfd	clients[64] = {0, 0, 0};
	// int clients_size = 0;
	//socklen_t test = 0;
	//in_addr test;
	// for (int i = 0; i < argc; ++i)
	// {
	// 	std::cout << argv[i] << std::endl;
	// }
	// memset(&result, 0 , sizeof(struct addrinfo));
	// memset(&hints, 0 , sizeof(struct addrinfo));
	// hints.ai_family = AF_INET;
	// hints.ai_socktype = SOCK_STREAM;// | SOCK_NONBLOCK;
	// hints.ai_flags = AI_PASSIVE;
	// hints.ai_protocol = 0;
	// hints.ai_canonname = NULL;
	// hints.ai_addr = INADDR_ANY;
	// hints.ai_next = NULL;
	
	// char buffer[1024] = {0};
	// std::string eval;

	//int returnGetaddrinfo = getaddrinfo(NULL, argv[1], &hints, &result);
	//std::cout << "getaddrinfo = " << returnGetaddrinfo << "\n";
	
	// int serverSocket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
	// std::cout << "socket = " << serverSocket << "\n";

 	//int returnSetsockopt = setsockopt(serverSocket, SOL_SOCKET, 0, NULL, 0);
	//std::cout << "setsockopt = " << returnSetsockopt << "\n";

	
	// int returnBind = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

	// std::cout << "bind = " << returnBind << "\n";
	// if (returnBind < 0)
	// {
	// 	std::cout << "error " << errno << "\n";
	// 	exit(-1);
	// }
	
	//int returnFcntl = fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFL, 0) | O_NONBLOCK );
	// int returnFcntl = fcntl(serverSocket, F_SETFL, O_NONBLOCK );
	// std::cout << "fcntl = " << returnFcntl << "\n";
	
	// int returnListen = listen(serverSocket, 5);
	// std::cout << "listen = " << returnListen << "\n";
	
	// struct sockaddr_in client;
	// std::memset(&client, 0, sizeof(client));
	// socklen_t client_number = sizeof(client);
	// int returnAccept;
	// int returnPoll;
	
	// while (true)
	// {
	// 	returnAccept = accept(serverSocket, (struct sockaddr *) &client, &client_number);
	// 	//std::cout << "accept = " << returnAccept << "\n";
	// 	if (returnAccept > 0)
	// 	{
	// 		std::cout << "accept = " << returnAccept << " client = " << ntohs(client.sin_port) << " - " << client.sin_family <<"\n";
	// 		clients[clients_size].fd = returnAccept;
	// 		clients[clients_size].events = POLLIN;
	// 		++clients_size;
	// 	}
	// 	returnPoll = poll(clients, clients_size, 500);
	// 	//std::cout << "poll = " << returnPoll << "\n";
	// 	if (returnPoll > 0)
	// 	{
	// 		for (int ii = 0; ii < clients_size; ++ii)
	// 		{
	// 			if ((clients[ii].revents & POLLIN) == POLLIN)
	// 			{
	// 				ssize_t returnRecv = recv(clients[ii].fd, buffer, 1024, 0);
	// 				buffer[returnRecv] = '\0';
	// 				//check for "\r\n" to mark end of the message otherwise append
	// 				if (returnRecv == 0)
	// 				{
	// 					close(clients[ii].fd);
	// 					clients[ii].fd = -1;
	// 					clients[ii].events = 0;
	// 					std::cout << "client disconnected\n";
	// 					break;
	// 				}
	// 				std::cout << "recv = " << returnRecv << " from fd = " << clients[ii].fd << " buffer = [" << buffer << "]\n";
	// 				Message newMessage(clients[ii].fd, buffer);
	// 				std::cout << newMessage.print_message();
	// 				for (int x = 0; x < clients_size; ++x)
	// 				{
	// 					if (x != ii)
	// 						send(clients[x].fd, buffer, std::strlen(buffer), MSG_DONTWAIT);
	// 				}
	// 				eval.assign(buffer);
	// 				if (eval.compare("exit\n") == 0)
	// 					break;
	// 			}
	// 		}
	// 	}
	// }
	//ssize_t returnSend = send(returnAccept, "ft_irc TEST!", 13, 0);
	//std::cout << "send = " << returnSend << "\n";
	//buffer[0] = '\0';
	
	// returnRecv = recv(returnAccept, buffer, 20, MSG_DONTWAIT);
	// std::cout << "recv = " << returnRecv << " buffer = " << buffer << "\n";
	// int returnClose = 0;
	// int x = clients_size;
	// for (int i = 0; i < x; ++i)
	// {
	// 	returnClose = close(clients[i].fd);
	// 	std::cout << "close = " << returnClose << "\n";
	// }
	return (0);
}