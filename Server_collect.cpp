/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_collect.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 17:05:43 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/29 15:44:05 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::collect_messages(void)
{
	char buffer[BUFFER_SIZE] = {0};
	ssize_t recv_size = 0;

	for (int i = 1; i < clients_size; ++i)
	{
		if ((clients_pollfd[i].revents & POLLIN) == POLLIN)
		{
			recv_size = recv(clients_pollfd[i].fd, buffer, BUFFER_SIZE - 1, 0);
			if (recv_size == 0 || recv_size > BUFFER_SIZE)
			{
				this->client_list.erase(clients_pollfd[i].fd);
				close(clients_pollfd[i].fd);
				std::cout << clients_pollfd[i].fd << " client disconnected\n";
				continue;
			}
			buffer[recv_size] = '\0';
			std::cout << "recv = " << recv_size << " fd = " << clients_pollfd[i].fd << " " << client_list.find(clients_pollfd[i].fd)->second.get_nickname() << " [" << buffer << "]\n";
			this->store_message(clients_pollfd[i].fd, buffer);
		}
	}
}


void	Server::store_message(int const & fd, char const * input)
{
	std::string	tmp = input;
	size_t		end_pos = 0;
	Client	&	that_client = client_list.find(fd)->second;

	do
	{
		end_pos = tmp.find("\r\n", 0); // \r || \n || \r\n
		if (end_pos == std::string::npos)
		{
			end_pos = tmp.find("\r", 0);
			if (end_pos == std::string::npos)
				end_pos = tmp.find("\n", 0);
		}
		
		if (end_pos == std::string::npos) //incomplete message append to client buffer
			that_client.append_message_buffer(tmp);
		else
		{
			that_client.append_message_buffer(tmp.substr(0, end_pos));
			tmp.erase(0, end_pos + 2);
			if (that_client.get_succesfully_reg() == true)
				received_message_queue.push(Message(fd, that_client.get_message_buffer()));
			else
				new_users_message_queue.push(Message(fd, that_client.get_message_buffer()));
			that_client.clear_message_buffer();
		}
	} while (end_pos != std::string::npos);
}
