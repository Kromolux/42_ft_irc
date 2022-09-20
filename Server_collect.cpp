/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_collect.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ehosu <ehosu@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 17:05:43 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/20 16:25:08 by ehosu            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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

void	Server::store_message(int const & fd, char const * input)
{
	std::string tmp = input;
	size_t start_pos = 0, end_pos = 0;
	size_t	end_r = 0, end_n = 0, end_rn = 0;
	std::map<int, Client>::iterator it = client_list.find(fd);
	do
	{
		end_rn = tmp.find("\r\n", start_pos); // || \r || \n || \r\n
		if (end_rn != std::string::npos)
			end_pos = end_rn;
		else
		{
			end_r = tmp.find("\r", start_pos);
			if (end_r != std::string::npos)
				end_pos = end_r;
			else
			end_n = tmp.find("\n", start_pos);
			if (end_n != std::string::npos)
				end_pos = end_n;
			else
				end_pos = std::string::npos;
		}
		
		if (end_pos == std::string::npos) //incomplete message
			it->second.append_message_buffer(tmp);
		else
		{
			it->second.append_message_buffer(tmp.substr(start_pos, end_pos));
			tmp.erase(start_pos, end_pos + 2);
			received_message_queue.push(Message(fd, it->second.get_message_buffer().c_str()));
			it->second.clear_message_buffer();
			start_pos = 0;
		}
	} while (end_pos != std::string::npos);
	
	this->received_message_list.insert(std::make_pair(fd, Message(fd, input)));
}
