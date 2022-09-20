/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 15:23:03 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/21 00:39:24 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int const & port, char const * password)
{
	this->port = port;
	this->password = password;
	this->server_name = "42.ft_irc";
	this->motd = "Welcome to our 42 Wolfsburg IRC server.";
	this->version = "ver. 0.0.1";
	this->user_modes = "no user modes";
	this->channel_modes = "no channel modes";
}

Server::Server(Server const & other) :hostname(other.hostname)
{}

Server & Server::operator=(Server const & rhs)
{
	//if (*this == rhs)
	//	return ;
	this->~Server();
	new (this) Server(rhs);
	return (*this);
}

Server::~Server(void)
{
	std::cout << "good bye!\n";
}

void	Server::init_server(void)
{
	clients_size = 0;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(this->port);

	this->serverSocket = socket(server_address.sin_family, SOCK_STREAM, 0);
	this->client_number = sizeof(client);
	std::cout << "socket = " << serverSocket << "\n";

	int enable = 1;
	int returnset = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	std::cout << "setsockopt = " << returnset << "\n";
	if (returnset < 0)
	{
		std::cerr << "error " << errno << "\n";
		std::cerr << strerror(errno) << "\n";
	}
	
	int returnBind = bind(serverSocket, (struct sockaddr *) &server_address, sizeof(server_address));
	std::cout << "bind = " << returnBind << "\n";
	if (returnBind < 0)
	{
		std::cout << "error " << errno << "\n";
		std::cerr << strerror(errno) << "\n";
		exit(-1);
	}

	int returnFcntl = fcntl(serverSocket, F_SETFL, O_NONBLOCK );
	std::cout << "fcntl = " << returnFcntl << "\n";
	
	int returnListen = listen(serverSocket, 5);
	std::cout << "listen = " << returnListen << "\n";
}

void	Server::run_server(void)
{
		returnAccept = accept(this->serverSocket, (struct sockaddr *) &client, &client_number);
		if (returnAccept > 0)
		{
			std::cout << "accept = " << returnAccept << " client = " << ntohs(client.sin_port) << " - " << client.sin_family <<"\n"
						<< inet_ntoa(client.sin_addr) << "\n";
			this->client_list.insert(std::make_pair(returnAccept, Client(returnAccept)));
			this->update_pollfd();
		}

		returnPoll = poll(clients, clients_size, 0);
		if (returnPoll > 0)
		{
			std::cout << "===< collect messages >===\n";
			this->collect_messages();
			std::cout << "===< process messages >===\n";
			this->process_messages();
			std::cout << "===< distribute messages >===\n";
			this->distribute_messages();
			std::cout << "===< wait for new messages >===\n";
			if (clients_size != static_cast<int>(client_list.size()))
				this->update_pollfd();
		}
}

void	Server::stop_server(void)
{
	for (int i = 0; i < clients_size; ++i)
		close(clients[i].fd);
}

void	Server::update_pollfd(void)
{
	std::map<int, Client>::iterator it = client_list.begin();
	clients_size = static_cast<int>(client_list.size());
	for (int i = 0; i < clients_size; ++i, ++it)
	{
		clients[i].fd = it->first;
		clients[i].events = POLLIN;
		clients[i].revents = 0;
	}
	std::cout << "client size = " << clients_size << "\n";
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

/*<nick>!<user>@<host>*/
std::string Server::get_nick_user_host_txt(int const & fd)
{
	Client	that_client = client_list.find(fd)->second;
	std::string nick = that_client.get_nickname();
	std::string user = that_client.get_username();
	std::string host = that_client.get_hostname();
	std::string tmp = nick + "!" + user + "@" + host;
	return (tmp);
}

void	Server::register_client(Message const & message)
{
	int fd = message.get_fd();
	server_code_nick_text_message(fd, "001", "Welcome to the Internet Relay Chat Network" + get_nick_user_host_txt(fd));
	server_code_nick_text_message(fd, "002", ("Your host is " + this->server_name + ", running version " + this->version) );
	server_code_nick_text_message(fd, "003", "This server was created September 2022");
	server_code_nick_text_message(fd, "004", (this->server_name + " " + this->version + " " + this->user_modes + " " + this->channel_modes) );
	MOTD(message);
	nick_user_host_message(fd, "MODE", "+i");
}

// std::string const Server::create_member_list_string(std::map<int, Client> const & client_list, std::set<int> const & member_list)
// {
// 	std::string tmp;
// 	std::set<int>::iterator it = member_list.begin();
// 	for (int i = 0, end = member_list.size(); i < end; ++i, ++it)
// 		tmp = tmp + client_list.find(*it)->second.get_nickname() + " ";
// 	return (tmp);
// }