/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_process.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 17:06:14 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/18 22:30:26 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::process_messages(void)
{
	//std::map<int, Message>::iterator it = received_message_list.begin();
	//std::map<int, Message>::iterator ite = received_message_list.end();
	const int	cmds_size = 29;
	std::string const CMDS[cmds_size] = {"PASS", "NICK", "USER", "JOIN", "PRIVMSG", "PING", "AWAY", "PART", \
	"WHOIS", "WHO", "WHOWAS", "NAMES", "MOTD", "RULES", "LUSERS", "MAP", "QUIT", "VERSION", "STATS", \
	"LINKS", "ADMIN", "INVITE", "KICK", "LIST", "NOTICE", "KNOCK", "SETNAME", "MODE", "SILENCE"};
	void	(Server::*f[cmds_size])(Message const &) = \
	{&Server::PASS, &Server::NICK, &Server::USER, &Server::JOIN, &Server::PRIVMSG, &Server::PING, &Server::AWAY, &Server::PART, \
	&Server::WHOIS, &Server::WHO, &Server::WHOWAS, &Server::NAMES, &Server::MOTD, &Server::RULES, &Server::LUSERS, \
	&Server::MAP, &Server::QUIT, &Server::VERSION, &Server::STATS, &Server::LINKS, &Server::ADMIN, &Server::INVITE, \
	&Server::KICK, &Server::LIST, &Server::NOTICE, &Server::KNOCK, &Server::SETNAME, &Server::MODE, &Server::SILENCE};

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

void	Server::create_error_message(Message const & message)
{
	std::cout << "ERROR msg created!\n";
	std::string nickname = client_list.find(message.get_fd())->second.get_nickname();
	std::string tmp = ":" + server_name + " 421 " + nickname + " " + message.get_cmd() + " :Unkown command!\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
}

void	Server::standard_message(Message const & message, std::string const & code, std::string const & postfix)
{
	std::string nickname = client_list.find(message.get_fd())->second.get_nickname();
	std::string tmp = ":" + server_name + " " + code + " " + nickname + " :" + postfix + "\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
}

void	Server::standard_message(Message const & message, std::string const & code, std::string const & text, std::string const & postfix)
{
	std::string nickname = client_list.find(message.get_fd())->second.get_nickname();
	std::string tmp = ":" + server_name + " " + code + " " + nickname +  " " + text + " :" + postfix + "\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
}