/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 15:23:03 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/17 15:05:47 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int const & port, char const * password)
{
	this->port = port;
	this->password = password;
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
	
	std::memset(&client, 0, sizeof(client));
	// this->Clients.insert(Client("rene"));
	// std::cout << "size of Clients: " << this->Clients.size() << "\n";
	// std::set<Client>::iterator it;
	// it = this->Clients.find(Client("rene"));
	// if (it != this->Clients.end())
	// 	std::cout << "found client: " << it->get_nickname() << "\n";
}

void	Server::start_server(void)
{
	int returnAccept;
	int returnPoll;

	//std::string eval;

	clients_size = 0;

	while (true)
	{
		returnAccept = accept(this->serverSocket, (struct sockaddr *) &client, &client_number);
		if (returnAccept > 0)
		{
			std::cout << "accept = " << returnAccept << " client = " << ntohs(client.sin_port) << " - " << client.sin_family <<"\n";
			this->add_new_client(returnAccept);
		}
		// else if (returnAccept == -1)
		// {
		// 	std::cerr << "accept error = " << errno << "\n";
		// }
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
		}
		if (clients_size != static_cast<int>(client_list.size()))
			update_pollfd();
	}
}

void	Server::collect_messages(void)
{
	char buffer[1024] = {0};
	ssize_t returnRecv = 0;

	for (int ii = 0; ii < clients_size; ++ii)
	{
		if ((clients[ii].revents & POLLIN) == POLLIN)
		{
			returnRecv = recv(clients[ii].fd, buffer, 1023, 0);
			buffer[returnRecv] = '\0';
			//check for "\r\n" to mark end of the message otherwise append
			if (returnRecv == 0)
			{
				this->remove_client(clients[ii].fd);
				//this->remove_message(clients[ii].fd);
				close(clients[ii].fd);
				std::cout << "client disconnected\n";
				continue;
			}
			std::cout << "recv = " << returnRecv << " from fd = " << clients[ii].fd << " buffer = [" << buffer << "]\n";
			this->store_message(clients[ii].fd, buffer);
			// Message newMessage(clients[ii].fd, buffer);
			// std::cout << newMessage.print_message();
			//send message
			// for (int x = 0; x < clients_size; ++x)
			// {
			// 	if (x != ii)
			// 		send(clients[x].fd, buffer, std::strlen(buffer), MSG_DONTWAIT);
			// }
		}
	}
}

void	Server::process_messages(void)
{
	//std::map<int, Message>::iterator it = received_message_list.begin();
	//std::map<int, Message>::iterator ite = received_message_list.end();
	int	cmds_size = 7;
	std::string const CMDS[cmds_size] = {"NICK", "USER", "JOIN", "PRIVMSG", "PING", "AWAY", "PART"};
	void	(Server::*f[cmds_size])(Message const &) = \
	{&Server::NICK, &Server::USER, &Server::JOIN, &Server::PRIVMSG, &Server::PING, &Server::AWAY, &Server::PART};

	// for (; it != ite; ++it)
	// {
	// 	//it->second.
	// 	for (int i = 0; i < cmds_size; ++i)
	// 	{
	// 		if (it->second.get_cmd() == CMDS[i])
	// 			(this->*f[i])(it->second);
	// 	}
	// }
	
	for (int i = 0, end = received_message_queue.size(); i < end; ++i)
	{
		Message process_message = received_message_queue.front();
		process_message.parse();
		process_message.print_message();
		bool	success = false;
		std::string cmd = process_message.get_cmd();
		for (int ii = 0; ii < cmds_size; ++ii)
		{
			if (cmd == CMDS[ii])
			{
				(this->*f[ii])(process_message);
				success = true;
				break;
			}
		}
		if (success == false)
			create_error_message(process_message);
		received_message_queue.pop();
	}
}

void	Server::update_pollfd(void)
{
	std::map<int, Client>::iterator it = client_list.begin();
	clients_size = static_cast<int>(client_list.size());
	for (int i = 0; i < clients_size; ++i)
	{
		clients[i].fd = it->first;
		clients[i].events = POLLIN;
		clients[i].revents = 0;
	}
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
	//++this->clients_size;
	//this->update_pollfd();
}

void	Server::remove_client(int const & fd)
{
	this->client_list.erase(fd);
	//--this->clients_size;
	//this->update_pollfd();
}

void	Server::store_message(int const & fd, char const * input)
{
	std::string tmp = input;
	size_t start_pos = 0, end_pos = 0;
	std::map<int, Client>::iterator it = client_list.find(fd);
	do
	{
		end_pos = tmp.find("\r\n", start_pos);
		if (end_pos == std::string::npos) //incomplete message
			it->second.append_message_buffer(input);
		else
		{
			it->second.append_message_buffer(tmp.substr(start_pos, end_pos));
			received_message_queue.push(Message(fd, it->second.get_message_buffer().c_str()));
			it->second.clear_message_buffer();
			start_pos = end_pos + 2;
		}
	} while (end_pos != std::string::npos);
	
	this->received_message_list.insert(std::make_pair(fd, Message(fd, input)));
}

void	Server::remove_message(int const & fd)
{
	this->received_message_list.erase(fd);
}

void	Server::distribute_messages(void)
{
	std::map<int, Message>::iterator it = send_message_list.begin();
	std::map<int, Message>::iterator ite = send_message_list.end();

	for (; it != ite; ++it)
	{
		if (it->second.receiver_is_channel())
		{
			std::map<std::string, Channel>::iterator channel = channel_list.find(it->second.get_receiver());
			if (channel != channel_list.end())
				send_channel_message(channel->second, it->second);
		}
		else
			send_message(it->second);
	}
}

void	Server::send_channel_message(Channel const & channel, Message const & message)
{
	std::set<int>::iterator it = channel.get_member_list().begin();
	std::set<int>::iterator ite = channel.get_member_list().end();
	int	sender = message.get_fd();
	for (; it != ite; ++it)
	{
		if (*it != sender)
			send(*it, message.get_raw(), message.get_len(), MSG_DONTWAIT);
	}
}

void	Server::send_message(Message const & message)
{
	send(get_client_fd(message.get_receiver()), message.get_raw(), message.get_len(), MSG_DONTWAIT);
}

void	Server::create_error_message(Message const & message)
{
	(void) message;
}

void	Server::PASS(Message const & message)
{
	(void) message;
}

void	Server::NICK(Message const & message)
{
	//check if nick already exists and throw error message
	if (get_client_fd(message.get_arg()) == -1)
	{
		std::map<int, Client>::iterator it = client_list.find(message.get_fd());
		it->second.set_nickname(message.get_arg());
		//create success respond message to sender
	}
	//else
		//create fail respond message to sender
	//set nickname
	
}

void	Server::USER(Message const & message)
{
	(void) message;
}

void	Server::JOIN(Message const & message)
{
	//validity check for channel name
	std::map<std::string, Channel>::iterator channel = channel_list.find(message.get_arg());
	if (channel != channel_list.end())
		channel->second.add_member(message.get_fd());
	else
		channel_list.insert(std::make_pair(message.get_arg(), Channel(message.get_arg(), message.get_fd())));
	//check if channel exists
	//add client to channel member list
}

void	Server::PRIVMSG(Message const & message)
{
	(void) message;
}

void	Server::PING(Message const & message)
{
	(void) message;
}

void	Server::AWAY(Message const & message)
{
	(void) message;
}

void	Server::PART(Message const & message)
{
	(void) message;
}
