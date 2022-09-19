/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 15:23:03 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/19 08:41:00 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int const & port, char const * password)
{
	this->port = port;
	this->password = password;
	this->server_name = "42_ft_irc";
	this->motd = "Welcome to our IRC server.";
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

	int returnBind = bind(serverSocket, (struct sockaddr *) &server_address, sizeof(server_address));
	std::cout << "bind = " << returnBind << "\n";
	if (returnBind < 0)
	{
		std::cout << "error " << errno << "\n";
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
			std::cout << "accept = " << returnAccept << " client = " << ntohs(client.sin_port) << " - " << client.sin_family <<"\n";
			//			<< client.sin_addr.s_addr << "\n";
			this->add_new_client(returnAccept);
			this->update_pollfd();
		}

		returnPoll = poll(clients, clients_size, 500);
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
	return (-1);
}

void	Server::add_new_client(int const & fd)
{
	this->client_list.insert(std::make_pair(fd, Client(fd)));
	std::string tmp = ":" + server_name + " NOTICE *:*** Looking up your hostname\r\n";
	send_message(Message(fd, tmp));
	tmp = ":" + server_name + " NOTICE *:*** Checking Ident\r\n";
	send_message(Message(fd, tmp));
	tmp = ":" + server_name + " NOTICE *:*** No Ident response\r\n";
	send_message(Message(fd, tmp));
	tmp = ":" + server_name + " NOTICE *:*** Couldn't look up your hostname\r\n";
	send_message(Message(fd, tmp));
	//++this->clients_size;
	//this->update_pollfd();
}

void	Server::remove_client(int const & fd)
{
	this->client_list.erase(fd);
	//--this->clients_size;
	//this->update_pollfd();
}

void	Server::remove_message(int const & fd)
{
	this->received_message_list.erase(fd);
}

std::string const Server::create_member_list_string(std::map<int, Client> const & client_list, std::set<int> const & member_list)
{
	std::string tmp;
	std::set<int>::iterator it = member_list.begin();
	for (int i = 0, end = member_list.size(); i < end; ++i, ++it)
		tmp = tmp + client_list.find(*it)->second.get_nickname() + " ";
	return (tmp);
}