/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 15:23:04 by rkaufman          #+#    #+#             */
/*   Updated: 2022/10/03 16:41:05 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#include <cstdlib>

#include <iostream>
#include <sstream>
#include <cstring>

#include <map>
#include <queue>
#include <vector>

#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"

#include "colors.h"

/*
	channel operator priviliges:
	1	can change the topic TOPIC <topic>
	2	can kick a user from the channel KICK <channel> <nick>
	3	can ban/unban a user from a channel MODE +b <nick> ; MODE -b <nick>
	4	can send channel invitations to other users INVITE <channel> <nick>
	5	can grand operator priviliges to other users MODE +o <nick> ; MODE -o <nick>
	6	can change the channel to invite only MODE <channel> +i ; MODE <channel> -i
	7	can change the channel flag 't', so that everyone can change the topic MODE <channel> -t MODE <channel> +t
	8	can change the channel flag 'n', so that everyone from outside the channel can send msg to the channel members MODE <channel> -n MODE <channel> +n

	bot commands
	1	hello
	2	weather
	3	joke
	4	time
	5	ping
*/
		extern bool server_run;

class Server{
	
	public:

		Server(int const & port, char const * password);
		~Server(void);

		int		init_server(void);
		void	run_server(void);
		void	stop_server(void);

	private:

		Server(Server const & other);
		Server & operator=(Server const & rhs);


		static const int		MAX_CLIENTS = 64;
		static const ssize_t	BUFFER_SIZE = 1024;

		std::string				server_name;
		std::string				password;
		std::string				operator_password;
		std::string				hostname;
		std::string				version;
		std::string				user_modes;
		std::string				channel_modes;
		std::string				motd;
		struct sockaddr_in		server_address;
		struct sockaddr_in		client;
		struct pollfd			clients_pollfd[MAX_CLIENTS];
		socklen_t				client_number;
		int						server_socket;
		int						clients_size;
		int						return_accept;
		int						port;

		std::map<int, Client>	client_list;

		std::queue<Message>		new_users_message_queue;
		std::queue<Message>		received_message_queue;
		std::queue<Message>		send_message_queue;

		std::map<std::string, Channel> channel_list;

		int		return_error(std::string const & error_text);

		void	collect_messages(void);
		void	process_messages(void);
		void	distribute_messages(void);

		void	update_pollfd(void);

		std::map<int, Client>::iterator	get_client_by_nick(std::string const & nick);
		int		get_client_fd(std::string const & nick_name);
		Client *get_client_obj(int const & fd);
		bool	is_nick_available(std::string const & nick);
		void	register_client(Message const & message);
		
		void	store_message(int const & fd, char const * input);

		void	send_channel_message(Channel const & channel, Message const & message);
		void	send_message(Message const & message);

		std::string	get_nick_user_host_txt(int const & fd);

		void	server_code_nick_text_message(int const & fd, std::string const & code, std::string const & text = "", std::string const & postfix = "");
		void	server_code_server_text_message(int const & fd, std::string const & code, std::string const & text = "", std::string const & postfix = "");
		void	server_code_text_message(int const & fd, std::string const & code, std::string const & text, std::string const & postfix = "");
		void	server_code_message(std::string const & nick_server, int const & fd, std::string const & code, std::string const & text, std::string const & postfix);
		void 	nick_user_host_message(int const & fd, std::string const & code, std::string const & postfix = "", std::string const & receiver = "");
		
		int		check_args_count(Message const & message, size_t const & args_count);
		int		check_authentication(int const & fd);
		int		check_registration(int const & fd);
		int		check_channel_exists(int const & fd, std::string const & channel_name);
		int		check_client_in_channel(int const & fd, std::string const & channel_name);
		int		check_nick_exists(int const & fd, std::string const & check_nick);
		int		check_nick_in_channel(int const & fd, std::string const & channel_name, std::string const & nick_name);
		int		check_client_is_operator(int const & fd, std::string const & channel_name);
		int		check_nick_is_banned(int const & fd, std::string const & channel_name, std::string const & nick_name);
		int		check_channel_name(int const & fd, std::string const & channel_name);
		int		check_invite_only(int const & fd, std::string const & channel_name, std::string const & nick);
		int		check_client_already_in_channel(int const & fd, std::string const & nick_name, std::string const & channel_name);
		bool	is_invalid_channel_name(std::string const & channel_name);
		
		//commands
		void	PASS(Message const & message);
		void	NICK(Message const & message);
		void	USER(Message const & message);
		void	JOIN(Message const & message);
		void	PRIVMSG(Message const & message);
		void	PING(Message const & message);
		void	PONG(Message const & message);
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
		void	TOPIC(Message const & message);
		void	ISON(Message const & message);
		void	DIE(Message const & message);
		
		void	not_implemented_yes(Message const & message);
		
		void	PRIVMSG_NOTICE(Message const & message, std::string const & type);

		//ircbot
		void	bot_populate_reply_map(void);
		void	bot_msg(Message const & message);

		std::map<std::string, std::vector<std::string> > bot_reply_map;
		size_t	bot_reply;
};

template <typename T>
std::string number_to_string ( T number )
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}
