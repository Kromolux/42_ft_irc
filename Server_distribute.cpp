/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_distribute.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 17:07:38 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/21 00:26:29 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::distribute_messages(void)
{
	for (int i = 0, end = send_message_queue.size(); i < end; ++i)
	{
		Message message_to_send = send_message_queue.front();
		//message_to_send.print_message();
		if (message_to_send.receiver_is_channel())
		{
			std::map<std::string, Channel>::iterator channel = channel_list.find(message_to_send.get_receiver());
			if (channel != channel_list.end())
				send_channel_message(channel->second, message_to_send);
		}
		else
			send_message(message_to_send);
		send_message_queue.pop();
	}
}

void	Server::send_channel_message(Channel const & channel, Message const & message)
{
	std::map<int, std::string>::const_iterator it = channel.get_member_list().begin();
	std::map<int, std::string>::const_iterator ite = channel.get_member_list().end();
	int	sender = message.get_fd();
	for (; it != ite; ++it)
	{
		if (it->first != sender)
		{
			send(it->first, message.get_raw(), message.get_len(), MSG_DONTWAIT);
			std::cout << server_name << ": send channel message to fd: " << message.get_fd() << " [" << message.get_raw();
		}
	}
}

void	Server::send_message(Message const & message)
{
	std::cout << server_name << ": send message to fd: " << message.get_fd() << " [" << message.get_raw();
	send(message.get_fd(), message.get_raw(), message.get_len(), MSG_DONTWAIT);
}
