/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_cmds.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 11:29:24 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/18 21:29:34 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::PASS(Message const & message)
{
	not_implemented_yes(message);
}

//NICK <nickname> [ <hopcount> ]
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

//USER <username> <hostname> <servername> :<realname>
void	Server::USER(Message const & message)
{
	//not_implemented_yes(message);
	client_list.find(message.get_fd())->second.set_realname(message.get_postfix());
	client_list.find(message.get_fd())->second.set_username(message.get_arg());
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
	//successfully joined a channel server reponse the user in the channel.
	std::string nick = client_list.find(message.get_fd())->second.get_nickname();
	std::string user = client_list.find(message.get_fd())->second.get_username();
	std::string host = client_list.find(message.get_fd())->second.get_hostname();
	std::string tmp = ":" + nick + "!" + user + "@" + host + " " + message.get_cmd() + " :" + message.get_arg() + "\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
	std::set<int> member_list = channel_list.find(message.get_arg())->second.get_member_list();
	std::string list = create_member_list_string(client_list, member_list);
	standard_message(message, "365", "= " + message.get_arg(), list);
	standard_message(message, "366", message.get_arg() + " " + " :End of /NAMES list.");
}

void	Server::PRIVMSG(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::PING(Message const & message)
{
	std::string tmp = ":" + server_name + " PONG " + server_name + " :" + message.get_arg() + "\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
}

void	Server::AWAY(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::PART(Message const & message)
{
	not_implemented_yes(message);
}


void	Server::WHOIS(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::WHO(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::WHOWAS(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::NAMES(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::MOTD(Message const & message)
{
	standard_message(message, "375", "Message of the Day -");
	standard_message(message, "372", this->motd);
	standard_message(message, "376", "End of /MOTD command.");
}

void	Server::RULES(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::LUSERS(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::MAP(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::QUIT(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::VERSION(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::STATS(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::LINKS(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::ADMIN(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::INVITE(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::KICK(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::LIST(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::NOTICE(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::KNOCK(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::SETNAME(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::MODE(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::SILENCE(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::not_implemented_yes(Message const & message)
{
	std::cout << "not implemented yet!\n";
	std::string tmp = ":" + server_name + " " + message.get_cmd() + " " + server_name + " :not implemented yet!\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
}