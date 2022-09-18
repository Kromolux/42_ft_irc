/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 15:23:04 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/18 21:28:35 by rkaufman         ###   ########.fr       */
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
		void	run_server(void);
		void	stop_server(void);

	private:
		std::string server_name;
		struct sockaddr_in server_address;
		int			port;
		std::string	password;
		std::string hostname;
		std::string motd;
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
		int returnAccept;
		int returnPoll;
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
		void	standard_message(Message const & message, std::string const & code, std::string const & postfix);
		void	standard_message(Message const & message, std::string const & code, std::string const & text, std::string const & postfix);
		std::string const create_member_list_string(std::map<int, Client> const & client_list, std::set<int> const & member_list);
		
		//commands
		void	PASS(Message const & message);
		void	NICK(Message const & message);
		void	USER(Message const & message);
		void	JOIN(Message const & message);
		void	PRIVMSG(Message const & message);
		void	PING(Message const & message);
		void	AWAY(Message const & message);
		void	PART(Message const & message);
		void	WHOIS(Message const & message);
		void	WHO(Message const & message);
		void	WHOWAS(Message const & message);
		void	NAMES(Message const & message);
		void	MOTD(Message const & message);
		void	RULES(Message const & message);
		void	LUSERS(Message const & message);
		void	MAP(Message const & message);
		void	QUIT(Message const & message);
		void	VERSION(Message const & message);
		void	STATS(Message const & message);
		void	LINKS(Message const & message);
		void	ADMIN(Message const & message);
		void	INVITE(Message const & message);
		void	KICK(Message const & message);
		void	LIST(Message const & message);
		void	NOTICE(Message const & message);
		void	KNOCK(Message const & message);
		void	SETNAME(Message const & message);
		void	MODE(Message const & message);
		void	SILENCE(Message const & message);
		
		void	not_implemented_yes(Message const & message);
};