/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_cmds.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 11:29:24 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/28 14:10:38 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* PASS <password> */
void	Server::PASS(Message const & message)
{
	if (check_args(message, 1) == EXIT_FAILURE)
		return ;
	
	if (message.get_args().at(0) == this->password)
	{
		std::map<int, Client>::iterator client_it = client_list.find(message.get_fd());
		client_it->second.set_authenticated(true);
	}
}

/*NICK <nickname> [ <hopcount> ]*/
void	Server::NICK(Message const & message)
{
	if (check_args(message, 1) == EXIT_FAILURE)
		return ;

	std::string	nick = message.get_args().at(0);
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
		server_code_nick_text_message(fd, "433", "= " + nick, "Nickname is already in use");
	}
}

/*USER <username> <hostname> <servername> :<realname> */
void	Server::USER(Message const & message)
{
	if (check_args(message, 4) == EXIT_FAILURE)
		return ;

	int	const fd = message.get_fd();
	Client & that_client = client_list.find(fd)->second;
	if (that_client.get_succesfully_reg() == false)
	{
		that_client.set_username(message.get_args().at(0));
		that_client.set_realname(message.get_postfix());
		if (that_client.get_nickname().size() > 0)
		{
			that_client.set_succesfully_reg(true);
			register_client(message);
		}
	}
	else
	{
		server_code_nick_text_message(fd, "ZZZ", "= " + message.get_args().at(0), "Error: User already registered!");
	}
	
}

/* JOIN <channel>{,<channel>} [<key>{,<key>}] */
void	Server::JOIN(Message const & message)
{
	if (check_args(message, 1) == EXIT_FAILURE)
		return ;

	//@ToDo validity check for channel names

	int			fd = message.get_fd();
	std::string	arg0 = message.get_args().at(0);
	std::string nick = client_list.find(fd)->second.get_nickname();
	//:42.ft_irc.local 479 Rene channel :Illegal channel name
	if (check_channel_name(fd, arg0) == EXIT_FAILURE)
		return ;


	std::map<std::string, Channel>::iterator channel = channel_list.find(arg0);
	if (channel == channel_list.end())
	{
		channel_list.insert(std::make_pair(arg0, Channel(arg0, fd)));
		channel = channel_list.find(arg0);
	}

	if (check_ban(fd, arg0, nick) == EXIT_FAILURE)
		return ;
	
	if (check_invite_only(fd, arg0, nick) == EXIT_FAILURE)
		return ;

	channel->second.add_member(fd, nick);

	nick_user_host_message(fd, message.get_cmd(), arg0);
	nick_user_host_message(fd, message.get_cmd(), arg0, arg0);

	if (channel->second.get_topic().empty() == false)
	{
		server_code_text_message(fd, "332", nick + " " + arg0, (channel->second.get_topic()));
	}
	
	server_code_text_message(fd, "MODE", (arg0 + " +nt") );
	NAMES(message);
	
	//std::string list = channel_list.find(arg0)->second.get_member_string_moderator();
	// server_code_nick_text_message(fd, "353", "= " + arg0, list);
	// server_code_nick_text_message(fd, "366", arg0, "End of /NAMES list.");

}

/* PRIVMSG <msgtarget> :<text to be sent> */
void	Server::PRIVMSG(Message const & message)
{
	PRIVMSG_NOTICE(message, "PRIVMSG");
}

void	Server::PING(Message const & message)
{
	if (check_args(message, 1) == EXIT_FAILURE)
		return ;
	server_code_server_text_message(message.get_fd(), "PONG", "", message.get_args().at(0));
}

void	Server::PONG(Message const & message)
{
	PING(message);
}

void	Server::AWAY(Message const & message)
{
	not_implemented_yes(message);
}

/* PART <channel>{,<channel>} */
void	Server::PART(Message const & message)
{
	if (check_args(message, 1) == EXIT_FAILURE)
		return ;

	if (check_channel(message.get_fd(), message.get_args().at(0)) == EXIT_FAILURE)
		return ;

	std::string	arg0 = message.get_args().at(0);
	std::map<std::string, Channel>::iterator	channel_it = channel_list.find(arg0);
	channel_it->second.remove_member(message.get_fd());
	//if channel is empty remove it from list
	nick_user_host_message(message.get_fd(), message.get_cmd() + " " + arg0);

	if (channel_it->second.get_member_list().size() == 0)
	{
		channel_list.erase(channel_it);
		return ;
	}
	nick_user_host_message(message.get_fd(), message.get_cmd() + " " + arg0, "", arg0);
}


void	Server::WHOIS(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::WHO(Message const & message)
{
	std::map<int, Client>::iterator client_it = client_list.begin();
	std::map<int, Client>::iterator client_ite = client_list.end();

	for (; client_it != client_ite; ++client_it)
	{
		server_code_nick_text_message(message.get_fd(), "352", "* " + client_it->second.get_username() + " " + client_it->second.get_hostname() + " " + this->server_name + " H", "0 " + client_it->second.get_realname());
	}

	server_code_nick_text_message(message.get_fd(), "315", "*", "End of /WHO list.");
}

void	Server::WHOWAS(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::NAMES(Message const & message)
{
	if (check_args(message, 1) == EXIT_FAILURE)
		return ;
	std::string arg0;
	std::string list;
	if (message.get_args().empty() == false)
	{
		arg0 = message.get_args().at(0);
		list = channel_list.find(arg0)->second.get_member_string_moderator();
	}
	server_code_nick_text_message(message.get_fd(), "353", "= " + arg0, list);
	server_code_nick_text_message(message.get_fd(), "366", arg0, "End of /NAMES list.");
}

void	Server::MOTD(Message const & message)
{
	int	fd = message.get_fd();
	server_code_nick_text_message(fd, "375", "", "Message of the Day -");
	server_code_nick_text_message(fd, "372", "", this->motd);
	server_code_nick_text_message(fd, "376", "", "End of /MOTD command.");
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
	//TODO: Check if it's doable with no arguments!
	if (check_args(message, 1) == EXIT_FAILURE)
		return ;
	int	fd = message.get_fd();

	std::map<std::string, Channel>::iterator channel_it = channel_list.begin();
	std::map<std::string, Channel>::iterator channel_ite = channel_list.end();
	for (; channel_it != channel_ite; ++channel_it)
	{
		if (channel_it->second.is_client_on_channel(fd) == true)
		{
			channel_it->second.remove_member(fd);
			nick_user_host_message(fd, "QUIT", message.get_postfix(), channel_it->first);
		}
	}

	client_list.erase(fd);
	close(fd);
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

/* INVITE <user> <channel> */
void	Server::INVITE(Message const & message)
{
	if (check_args(message, 2) == EXIT_FAILURE)
		return ;

	if (check_channel(message.get_fd(), message.get_args().at(1)) == EXIT_FAILURE)
		return ;

	if (check_client(message.get_fd(), message.get_args().at(0)) == EXIT_FAILURE)
		return ;

	if (check_client_operator(message.get_fd(), message.get_args().at(1)) == EXIT_FAILURE)
		return ;

	std::string	sender_nick = client_list.find(message.get_fd())->second.get_nickname();
	std::string	channel_name = message.get_args().at(1);
	std::string	nick_name = message.get_args().at(0);
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);

	channel_it->second.add_invite(nick_name);
	nick_user_host_message(message.get_fd(), "341 " + sender_nick + " " + nick_name + " " + channel_name);
	nick_user_host_message(get_client_fd(nick_name), message.get_cmd() + " " + nick_name, channel_name);
	//only to channel operators
	//nick_user_host_message(message.get_fd(), "341 " + sender_nick + " " + nick_name + " " + channel_name, channel_name);
}

/* KICK <channel> <user> [<comment>] */
void	Server::KICK(Message const & message)
{
	if (check_args(message, 2) == EXIT_FAILURE)
		return ;

	if (check_channel(message.get_fd(), message.get_args().at(0)) == EXIT_FAILURE)
		return ;

	if (check_client(message.get_fd(), message.get_args().at(1)) == EXIT_FAILURE)
		return ;

	if (check_nick_in_channel(message) == EXIT_FAILURE)
		return ;

	if (check_client_operator(message.get_fd(), message.get_args().at(0)) == EXIT_FAILURE)
		return ;

	std::string	sender_nick = client_list.find(message.get_fd())->second.get_nickname();
	std::string	channel_name = message.get_args().at(0);
	std::string	nick_name = message.get_args().at(1);
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);
	std::map<int, Client>::iterator client_it = get_client_by_nick(nick_name);
	
	channel_it->second.remove_member(client_it->first);

	nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " " + sender_nick, nick_name);
	nick_user_host_message(client_it->first, message.get_cmd() + " " + channel_name + " " + sender_nick, nick_name);
	nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " " + sender_nick, nick_name, channel_name);
}

void	Server::LIST(Message const & message)
{
	int sender_fd = message.get_fd();
	
	server_code_nick_text_message(sender_fd, "321", "Channel", "Users Name");

	std::map<std::string, Channel>::iterator channel_it = channel_list.begin();
	std::map<std::string, Channel>::iterator channel_ite = channel_list.end();
	std::string number_string;
	for (; channel_it != channel_ite ; ++channel_it)
	{
		number_string = number_to_string(channel_it->second.get_member_list().size());
		server_code_nick_text_message(sender_fd, "322", channel_it->first + " " + number_string, "[+nt]");
	}

	server_code_nick_text_message(sender_fd, "323", "", "End of /LIST");
}

/* NOTICE <msgtarget> <text> */
void	Server::NOTICE(Message const & message)
{
	PRIVMSG_NOTICE(message, "NOTICE");
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

/* MODE <nickname> {[+|-]|i|w|s|o}
* MODE <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>] [<ban mask>] */
void	Server::MODE(Message const & message)
{
	//TODO: Check this!
	if (check_args(message, 1) == EXIT_FAILURE)
		return ;

	if (check_channel(message.get_fd(), message.get_args().at(0)) == EXIT_FAILURE)
		return ;

	std::string	channel_name = message.get_args().at(0);
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);

	int argc = message.get_args().size();
	if (argc > 1)
	{

		if (check_client_operator(message.get_fd(), message.get_args().at(0)) == EXIT_FAILURE)
			return ;
			
		std::string flags = message.get_args().at(1);
		if (flags.find('-') != std::string::npos && flags.find('+') != std::string::npos)
			return ;

		if ( (flags.find('o') != std::string::npos) && (argc > 2) )
		{
			if (check_client(message.get_fd(), message.get_args().at(2)) == EXIT_FAILURE)
				return ;

			if (check_nick_in_channel(message) == EXIT_FAILURE)
				return ;

			std::string	nick_name = message.get_args().at(2);
			std::map<int, Client>::iterator client_it = get_client_by_nick(nick_name);

			if (flags.find('-') != std::string::npos)
			{
				channel_it->second.remove_operator(client_it->first);
			}
			else
			{
				channel_it->second.add_operator(client_it->first);
			}
			nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " " + flags + " " + nick_name);
			nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " " + flags + " " + nick_name, "", channel_name);
			//return ;
		}

		//:42.ft_irc.local 474 rene #42 :Cannot join channel (+b)
		if (flags.find('b') != std::string::npos)
		{
			if (check_client(message.get_fd(), message.get_args().at(2)) == EXIT_FAILURE)
				return ;

			std::string	nick_name = message.get_args().at(2);
			std::map<int, Client>::iterator client_it = get_client_by_nick(nick_name);

			if (flags.find('-') != std::string::npos)
			{
				channel_it->second.remove_ban(client_it->second.get_nickname());
			}
			else
			{
				channel_it->second.add_ban(client_it->second.get_nickname());
			}
			nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " " + flags + " " + nick_name);
			nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " " + flags + " " + nick_name, "", channel_name);
		}
		if (flags.find('i') != std::string::npos)
		{
			if (flags.find('-') != std::string::npos)
			{
				channel_it->second.set_channel_invite_only(false);
			}
			else
			{
				channel_it->second.set_channel_invite_only(true);
			}
			nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " " + flags);
			nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " " + flags, "", channel_name);
		}
		return ;
	}
	//not_implemented_yes(message);
	// :42.ft_irc.local 324 user42 #42 +nt 
	// :42.ft_irc.local 329 user42 #42 1663686893
	//std::string nick = client_list.find(fd)->second.get_nickname();
	//std::string tmp = ":" + server_name + " 324 " + nick + " " + message.get_arg() + " +nt\r\n";
	//send_message_queue.push(Message(fd, tmp));
	//@ToDo implement flags in return msg
	server_code_nick_text_message(message.get_fd(), "324", (message.get_args().at(0) + " +nt") );
	//tmp = ":" + server_name + " 329 " + nick + " " + message.get_arg() + "\r\n";
	//send_message_queue.push(Message(message.get_fd(), tmp));
	server_code_nick_text_message(message.get_fd(), "329", message.get_args().at(0));
}

void	Server::SILENCE(Message const & message)
{
	not_implemented_yes(message);
}

void	Server::not_implemented_yes(Message const & message)
{
	server_code_server_text_message(message.get_fd(), message.get_cmd(),"" ,"not implemented yet!");
}

void	Server::PRIVMSG_NOTICE(Message const & message, std::string const & type)
{
	if (check_args(message, 2) == EXIT_FAILURE)
		return ;

	std::string	arg0 = message.get_args().at(0);

	if (arg0[0] == '#')
	{
		if (check_channel(message.get_fd(), message.get_args().at(0)) == EXIT_FAILURE)
			return ;

		nick_user_host_message(message.get_fd(), type + " " +  arg0, message.get_postfix(), arg0);
		return ;
	}
	
	if (check_client(message.get_fd(), arg0) == EXIT_FAILURE)
		return ;

	std::map<int, Client>::iterator		it_client = get_client_by_nick(arg0);
	std::string tmp = ":" + get_nick_user_host_txt(message.get_fd()) + " " + type + " " +  arg0 + " :" + message.get_postfix() + "\r\n";
	send_message_queue.push(Message(it_client->first, tmp));
}

void	Server::TOPIC(Message const & message)
{
	if (check_args(message, 1) == EXIT_FAILURE)
		return ;

	if (check_channel(message.get_fd(), message.get_args().at(0)) == EXIT_FAILURE)
		return ;

	std::string	channel_name = message.get_args().at(0);
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);

	//if args > 1 then change topic
	if ( message.get_args().size() > 1 )
	{
		if (check_client_operator(message.get_fd(), message.get_args().at(0)) == EXIT_FAILURE)
			return ;
		channel_it->second.set_topic(message.get_args().at(1));
		nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " ", channel_it->second.get_topic());
		nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " ", channel_it->second.get_topic(), channel_name);
		return ;
	}
	
	nick_user_host_message(message.get_fd(), message.get_cmd() + " " + channel_name + " ", channel_it->second.get_topic());
}

