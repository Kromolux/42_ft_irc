/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 15:23:03 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/30 18:14:09 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int const & port, char const * password)
{
	this->port = port;
	this->password = password;
	this->server_name = "ft_irc@42wolfsburg.de";
	this->motd = "Welcome to our 42 Wolfsburg IRC server.";
	this->version = "ver. 0.0.1";
	this->user_modes = "i";
	this->channel_modes = "oitnb";
}

Server::Server(Server const & other) :hostname(other.hostname)
{}

Server & Server::operator=(Server const & rhs)
{
	this->~Server();
	new (this) Server(rhs);
	return (*this);
}


Server::~Server(void)
{
	std::cout << COLOR_GREEN << "IRC Server stopped and shut down...\n" << COLOR_DEFAULT;
}


int	Server::return_error(std::string const & error_text)
{
	std::cerr << COLOR_RED << "ERROR! " << error_text << errno << "\n" << COLOR_DEFAULT;
	return (EXIT_FAILURE);
}


int	Server::init_server(void)
{
	this->clients_size = 1;
	this->server_address.sin_family = AF_INET;
	this->server_address.sin_addr.s_addr = INADDR_ANY;
	this->server_address.sin_port = htons(this->port);
	this->client_number = sizeof(client);

	this->server_socket = socket(server_address.sin_family, SOCK_STREAM, 0);

	if (server_socket < 0)
		return ( return_error("socket returned ") );

	int enable = 1;
	//@ToDo figure out how to realy reuse address and port after closing/crash
	if ( setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0 )
		return ( return_error("setsockopt returned ") );

	if ( setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0 )
		return ( return_error("setsockopt returned ") );

	if ( bind(this->server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0 )
		return ( return_error("bind returned ") );

	//if ( fcntl(this->server_socket, F_SETFL, O_NONBLOCK ) < 0 )
	//	return ( return_error("fcntl returned ") );

	if ( listen(this->server_socket, 5) < 0 )
		return ( return_error("listen returned ") );

	update_pollfd();
	ircbot_populate_reply_map();
	return ( EXIT_SUCCESS );
}


void	Server::run_server(void)
{

	if (poll(clients_pollfd, clients_size, 0) > 0)
	{
		if (clients_pollfd[0].revents == POLLIN)
		{
			this->return_accept = accept(this->server_socket, (struct sockaddr *) &client, &client_number);
			if (this->return_accept > 0)
			{
				#if (DEBUG)
					std::cout << COLOR_GREEN << "new client connected accept = " << this->return_accept << " IP = " << inet_ntoa(client.sin_addr) << "\n" << COLOR_DEFAULT; //@ToDo how to get the client hostname?
				#endif
				if (clients_size < MAX_CLIENTS)
				{
					std::map<int, Client>::iterator client_it;
					client_it = (client_list.insert(std::make_pair(this->return_accept, Client(this->return_accept)))).first;
					client_it->second.set_hostname(std::string(inet_ntoa(client.sin_addr)));
					update_pollfd();
				}
				else
				{
					std::string tmp = ":" + this->server_name + " 519 * :Too many clients on server, try again later\r\n";
					#if (DEBUG)
						std::cout << COLOR_CYAN << "send msg => " << return_accept << COLOR_YELLOW << tmp << COLOR_DEFAULT;
					#endif
					send(return_accept, tmp.c_str(), tmp.length(), MSG_DONTWAIT);
					close(return_accept);
				}
			}
		}
		#if (DEBUG)
			std::cout << COLOR_YELLOW << "===< collect messages >===\n" << COLOR_DEFAULT;
		#endif
		this->collect_messages();
		#if (DEBUG)
			std::cout << COLOR_YELLOW << "===< process messages >===\n" << COLOR_DEFAULT;
		#endif
		this->process_messages();
		#if (DEBUG)
			std::cout << COLOR_YELLOW << "===< distribute messages >===\n" << COLOR_DEFAULT;
		#endif
		this->distribute_messages();
		#if (DEBUG)
			std::cout << COLOR_YELLOW << "===< wait for new messages >===\n" << COLOR_DEFAULT;
		#endif
		if (clients_size != static_cast<int>(client_list.size()) + 1)
			update_pollfd();
	}
}


void	Server::stop_server(void)
{
	for (int i = 0; i < clients_size; ++i)
		close(clients_pollfd[i].fd);
}


void	Server::update_pollfd(void)
{
	std::map<int, Client>::iterator it = client_list.begin();
	clients_size = static_cast<int>(client_list.size()) + 1;
	
	clients_pollfd[0].fd = this->server_socket;
	clients_pollfd[0].events = POLLIN;
	clients_pollfd[0].revents = 0;

	for (int i = 1; i < clients_size; ++i, ++it)
	{
		clients_pollfd[i].fd = it->first;
		clients_pollfd[i].events = POLLIN;
		clients_pollfd[i].revents = 0;
	}
	//std::cout << "client size = " << clients_size << "\n";
}


int	Server::get_client_fd(std::string const & nickname)
{
	std::map<int, Client>::iterator it = client_list.begin();
	std::map<int, Client>::iterator ite = client_list.end();
	for (; it != ite; ++it)
	{
		if (it->second.get_nickname() == nickname)
			return (it->first);
	}
	return (0);
}


Client *Server::get_client_obj(int const & fd)
{
	std::map<int, Client>::iterator client_it = client_list.find(fd);
	if (client_it != client_list.end())
		return (&client_it->second);
	return (NULL);
}


bool	Server::is_nick_available(std::string const & nick)
{
	return (!get_client_fd(nick));
}

/*
001 "Welcome to the Internet Relay Network <nick>!<user>@<host>"
002 "Your host is <servername>, running version <ver>"
003 "This server was created <date>"
004 "<servername> <version> <available user modes> <available channel modes>"

:42.ft_irc.local 001 Rene :Welcome to the debian Internet Relay Chat Network Rene!Rene@127.0.0.1
:42.ft_irc.local 002 Rene :Your host is 42.ft_irc.local[0.0.0.0/6667], running version hybrid-1:8.2.26+dfsg.1-1
:42.ft_irc.local 003 Rene :This server was created September 1 2019 at 11:18:31+0000
:42.ft_irc.local 004 Rene 42.ft_irc.local hybrid-1:8.2.26+dfsg.1-1 DFGHXRSWabcdefgijklnopqrsuwy bchiklmnoprstuveCILMNORST bkloveIh
*/
void	Server::register_client(Message const & message)
{
	int fd = message.get_fd();
	server_code_nick_text_message(fd, "001", "Welcome to the Internet Relay Chat Network " + get_nick_user_host_txt(fd));
	server_code_nick_text_message(fd, "002", ("Your host is " + this->server_name + ", running version " + this->version) );
	server_code_nick_text_message(fd, "003", "This server was created September 2022");
	server_code_nick_text_message(fd, "004", (this->server_name + " " + this->version + " " + this->user_modes + " " + this->channel_modes) );
	MOTD(message);
	nick_user_host_message(fd, "MODE " + this->client_list.find(fd)->second.get_nickname(), "+i");
}


std::map<int, Client>::iterator	Server::get_client_by_nick(std::string const & nick)
{
	std::map<int, Client>::iterator	it = client_list.begin();
	std::map<int, Client>::iterator	ite = client_list.end();

	for (; it != ite; ++it)
	{
		if (it->second.get_nickname() == nick)
			return (it);
	}
	return (it);
}
