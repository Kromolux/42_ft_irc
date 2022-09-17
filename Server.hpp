/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 15:23:04 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/17 14:05:33 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <poll.h>

#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <string.h>

#include <errno.h>
#include <stdlib.h>
#include <vector>

#include <cstring>
#include <queue>
#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"

class Server{
	
	public:

		Server(int const & port, char const * password);
		Server(Server const & other);
		Server & operator=(Server const & rhs);
		~Server(void);

		void	init_server(void);
		void	start_server(void);

	private:

		struct sockaddr_in server_address;
		int			port;
		std::string	password;
		std::string hostname;
		//clients in a map?
		std::map<int, Client> client_list;
		std::queue<Message> received_message_queue;
		std::queue<Message> send_message_queue;
		std::map<int, Message> received_message_list;
		std::map<int, Message> send_message_list;
		std::map<std::string, Channel> channel_list;
		struct pollfd	clients[64];
		int serverSocket;
		struct sockaddr_in client;
		socklen_t client_number;
		int clients_size;
		//channel in map?
		//setup info

		void	collect_messages(void);
		void	update_pollfd(void);
		int		get_client_fd(std::string const & nickname);
		void	add_new_client(int const & fd);
		void	remove_client(int const & fd);
		void	store_message(int const & fd, char const * input);
		void	remove_message(int const & fd);
		void	process_messages(void);
		void	distribute_messages(void);
		void	send_channel_message(Channel const & channel, Message const & message);
		void	send_message(Message const & message);
		void	create_error_message(Message const & message);
		
		//commands
		void	PASS(Message const & message);
		void	NICK(Message const & message);
		void	USER(Message const & message);
		void	JOIN(Message const & message);
		void	PRIVMSG(Message const & message);
		void	PING(Message const & message);
		void	AWAY(Message const & message);
		void	PART(Message const & message);
};