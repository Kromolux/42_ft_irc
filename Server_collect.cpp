/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_collect.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 17:05:43 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/20 23:30:14 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::collect_messages(void)
{
	char buffer[BUFFER_SIZE] = {0};
	ssize_t recv_size = 0;

	for (int ii = 0; ii < clients_size; ++ii)
	{
		if ((clients[ii].revents & POLLIN) == POLLIN)
		{
			recv_size = recv(clients[ii].fd, buffer, BUFFER_SIZE - 1, 0);
			if (recv_size == 0 || recv_size > BUFFER_SIZE)
			{
				this->client_list.erase(clients[ii].fd);
				close(clients[ii].fd);
				std::cout << "client disconnected\n";
				continue;
			}
			buffer[recv_size] = '\0';
			std::cout << "recv = " << recv_size << " from fd = " << clients[ii].fd << " buffer = [" << buffer << "]\n";
			this->store_message(clients[ii].fd, buffer);
		}
	}
}

void	Server::store_message(int const & fd, char const * input)
{
	std::string tmp = input;
	size_t end_pos = 0;
	Client that_client = client_list.find(fd)->second;
	do
	{
		end_pos = tmp.find("\r\n", 0); // || \r || \n || \r\n
		if (end_pos == std::string::npos)
		{
			end_pos = tmp.find("\r", 0);
			if (end_pos == std::string::npos)
				end_pos = tmp.find("\n", 0);
		}
		
		if (end_pos == std::string::npos) //incomplete message
			that_client.append_message_buffer(tmp);
		else
		{
			that_client.append_message_buffer(tmp.substr(0, end_pos));
			tmp.erase(0, end_pos + 2);
			received_message_queue.push(Message(fd, that_client.get_message_buffer()));
			that_client.clear_message_buffer();
		}
	} while (end_pos != std::string::npos);
}
