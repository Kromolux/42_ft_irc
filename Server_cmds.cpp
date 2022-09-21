/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_cmds.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 11:29:24 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/21 17:00:27 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::PASS(Message const & message)
{
	not_implemented_yes(message);
}

/*NICK <nickname> [ <hopcount> ]*/
void	Server::NICK(Message const & message)
{
	std::string	nick = message.get_arg();
	int	const	fd = message.get_fd();

	//check if nick already exists and throw error message
	if (is_nick_available(nick))
	{
		Client & that_client = client_list.find(fd)->second;
		that_client.set_nickname(nick);
		if (that_client.get_username().size() > 0 && that_client.get_succesfully_reg() == false)
		{
			that_client.set_succesfully_reg(true);
			register_client(message);
		}
	}
	else
	{
		server_code_nick_text_message(fd, "433", "= " + message.get_arg(), "Nickname is already in use");
	}
}

/*USER <username> <hostname> <servername> :<realname> */
void	Server::USER(Message const & message)
{
	int	const fd = message.get_fd();
	Client & that_client = client_list.find(fd)->second;
	if (that_client.get_succesfully_reg() == false)
	{
		that_client.set_username(message.get_arg());
		that_client.set_realname(message.get_postfix());
		if (that_client.get_nickname().size() > 0)
		{
			that_client.set_succesfully_reg(true);
			register_client(message);
		}
	}
	else
	{
		server_code_nick_text_message(fd, "ZZZ", "= " + message.get_arg(), "Error: User already registered!");
	}
	
}

void	Server::JOIN(Message const & message)
{
	std::string	arg = message.get_arg();
	int			fd = message.get_fd();

	//validity check for channel name
	std::map<std::string, Channel>::iterator channel = channel_list.find(arg);
	if (channel == channel_list.end())
	{
		channel_list.insert(std::make_pair(arg, Channel(arg, fd)));
		channel = channel_list.find(arg);
	}
	channel->second.add_member(fd, client_list.find(fd)->second.get_nickname());
	//check if channel exists
	//:user42!user42@i.love.debian.org JOIN :#42

	//std::string tmp = ":" + get_nick_user_host_txt(fd) + " " + message.get_cmd() + " :" + message.get_arg() + "\r\n";

	nick_user_host_message(fd, message.get_cmd(), arg);

	send_message_queue.push( Message(fd, ":" + get_nick_user_host_txt(fd) + " " + message.get_cmd() + " :" + arg + "\r\n", arg) );
	
	//:42.ft_irc.local MODE #42 +nt
	server_code_text_message(fd, "MODE", (arg + " +nt") );
	
	std::string list = channel_list.find(arg)->second.get_member_string();
	server_code_nick_text_message(fd, "353", "= " + arg, list);
	server_code_nick_text_message(fd, "366", arg, "End of /NAMES list.");
	//:user42!user42@i.love.debian.org MODE user42 :+i
}

void	Server::PRIVMSG(Message const & message)
{
	std::string tmp = ":" + get_nick_user_host_txt(message.get_fd()) + " " + message.get_cmd() + " " +  message.get_arg() + " :" + message.get_postfix() + "\r\n";
	std::map<std::string, Channel>::iterator	it_channel = channel_list.find(message.get_arg());
	std::map<int, Client>::iterator		it_client = get_client(message.get_arg());

	if (it_channel != channel_list.end())
		send_message_queue.push(Message(message.get_fd(), tmp, message.get_arg()));
	else if (it_client != client_list.end())
		send_message_queue.push(Message(it_client->first, tmp));
}

std::map<int, Client>::iterator	Server::get_client(std::string const & nick)
{
	std::map<int, Client>::iterator	it = client_list.begin();
	std::map<int, Client>::iterator	ite = client_list.end();

	for (; it != ite; ++it)
	{
		if (it->second.get_nickname() == nick)
			return (it);
	}
	return (it);
}

void	Server::PING(Message const & message)
{
	server_code_server_text_message(message.get_fd(), "PONG", "", message.get_arg());
}

void	Server::PONG(Message const & message)
{
	PING(message);
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
	int	fd = message.get_fd();
	server_code_nick_text_message(fd, "375", "Message of the Day -");
	server_code_nick_text_message(fd, "372", this->motd);
	server_code_nick_text_message(fd, "376", "End of /MOTD command.");
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

/*
   A user MODE command MUST only be accepted if both the sender of the
   message and the nickname given as a parameter are both the same.  If
   no other parameter is given, then the server will return the current
   settings for the nick.

      The available modes are as follows:

           a - user is flagged as away;
           i - marks a users as invisible;
           w - user receives wallops;
           r - restricted user connection;
           o - operator flag;
           O - local operator flag;
           s - marks a user for receipt of server notices.

   Additional modes may be available later on.
*/
void	Server::MODE(Message const & message)
{
	//not_implemented_yes(message);
	// :42.ft_irc.local 324 user42 #42 +nt 
	// :42.ft_irc.local 329 user42 #42 1663686893
	//std::string nick = client_list.find(fd)->second.get_nickname();
	//std::string tmp = ":" + server_name + " 324 " + nick + " " + message.get_arg() + " +nt\r\n";
	//send_message_queue.push(Message(fd, tmp));
	server_code_nick_text_message(message.get_fd(), "324", (message.get_arg() + " +nt") );
	//tmp = ":" + server_name + " 329 " + nick + " " + message.get_arg() + "\r\n";
	//send_message_queue.push(Message(message.get_fd(), tmp));
	server_code_nick_text_message(message.get_fd(), "329", message.get_arg());
}

void	Server::SILENCE(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::not_implemented_yes(Message const & message)
{
	server_code_server_text_message(message.get_fd(), message.get_cmd(),"" ,"not implemented yet!");
}
