/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_cmds.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 11:29:24 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/20 18:19:34 by rkaufman         ###   ########.fr       */
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
		if (it->second.get_username() != "")
		{
			it->second.set_succesfully_reg(true);
			register_client(message);
		}
		//create success respond message to sender
	}
	else
	{
		standard_message(message, "xxx", "= " + message.get_arg(), "Error: Nickname already registered!");
	}
	//set nickname
	
}

//USER <username> <hostname> <servername> :<realname>
void	Server::USER(Message const & message)
{
	std::map<int, Client>::iterator it = client_list.find(message.get_fd());
	if (it->second.get_succesfully_reg() == false)
	{
		//not_implemented_yes(message);
		client_list.find(message.get_fd())->second.set_realname(message.get_postfix());
		client_list.find(message.get_fd())->second.set_username(message.get_arg());
		//check for successfull registration
		// register_client(message);
		if (it->second.get_nickname() != "")
		{
			it->second.set_succesfully_reg(true);
			register_client(message);
		}
	}
	else
	{
		standard_message(message, "zzz", "= " + message.get_arg(), "Error: User already registered!");
	}
	
}

void	Server::JOIN(Message const & message)
{
	//validity check for channel name
	std::map<std::string, Channel>::iterator channel = channel_list.find(message.get_arg());
	if (channel == channel_list.end())
	{
		channel_list.insert(std::make_pair(message.get_arg(), Channel(message.get_arg(), message.get_fd())));
		channel = channel_list.find(message.get_arg());
	}
	channel->second.add_member(message.get_fd());
	//check if channel exists
	//:user42!user42@i.love.debian.org JOIN :#42

	//add client to channel member list
	//successfully joined a channel server reponse the user in the channel.
	std::string nick = client_list.find(message.get_fd())->second.get_nickname();
	std::string user = client_list.find(message.get_fd())->second.get_username();
	std::string host = client_list.find(message.get_fd())->second.get_hostname();
	std::string tmp = ":" + nick + "!" + user + "@" + host + " " + message.get_cmd() + " :" + message.get_arg() + "\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
	send_message_queue.push(Message(message.get_fd(), tmp, message.get_arg()));
	std::set<int> member_list = channel_list.find(message.get_arg())->second.get_member_list();
	std::string list = create_member_list_string(client_list, member_list);
	//:42.ft_irc.local MODE #42 +nt

	tmp = ":" + this->hostname + " MODE " + message.get_arg() + " +nt\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
	standard_message(message, "353", "= " + message.get_arg(), list);
	standard_message(message, "366", message.get_arg(), "End of /NAMES list.");
	//:user42!user42@i.love.debian.org MODE user42 :+i
	

}

void	Server::PRIVMSG(Message const & message)
{
	std::string nick = client_list.find(message.get_fd())->second.get_nickname();
	std::string user = client_list.find(message.get_fd())->second.get_username();
	std::string host = client_list.find(message.get_fd())->second.get_hostname();
	std::string tmp = ":" + nick + "!" + user + "@" + host + " " + message.get_cmd() + " " +  message.get_arg() + " :" + message.get_postfix() + "\r\n";
	std::map<std::string, Channel>::iterator channel = channel_list.find(message.get_arg());
	if (channel != channel_list.end())
		send_message_queue.push(Message(message.get_fd(), tmp, message.get_arg()));
}

void	Server::PING(Message const & message)
{
	std::string tmp = ":" + server_name + " PONG " + server_name + " :" + message.get_arg() + "\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
}

void	Server::PONG(Message const & message)
{
	std::string tmp = ":" + server_name + " PING " + server_name + " :" + message.get_arg() + "\r\n";
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
	//not_implemented_yes(message);
	// :42.ft_irc.local 324 user42 #42 +nt 
	// :42.ft_irc.local 329 user42 #42 1663686893
	std::string nick = client_list.find(message.get_fd())->second.get_nickname();
	std::string tmp = ":" + server_name + " 324 " + nick + " " + message.get_arg() + " +nt\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
	tmp = ":" + server_name + " 329 " + nick + " " + message.get_arg() + "\r\n";
	send_message_queue.push(Message(message.get_fd(), tmp));
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