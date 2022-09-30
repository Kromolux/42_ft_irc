/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_cmds.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 11:29:24 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/30 17:40:49 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* PASS <password> */
void	Server::PASS(Message const & message)
{
	if (check_args_count(message, 1) == EXIT_FAILURE)
		return ;
	
	if (message.get_args().at(0) == this->password)
	{
		std::map<int, Client>::iterator client_it = client_list.find(message.get_fd());
		if (client_it != client_list.end())
			client_it->second.set_authenticated(true);
	}
}


/*NICK <nickname> [ <hopcount> ]*/
void	Server::NICK(Message const & message)
{
	if (check_args_count(message, 1) == EXIT_FAILURE)
		return ;

	int	const &			fd = message.get_fd();
	std::string	const &	nick = message.get_args().at(0);

	//check if nick already exists and throw error message
	if (is_nick_available(nick))
	{
		Client * that_client = get_client_obj(fd);
		if (that_client == NULL)
			return ;

		that_client->set_nickname(nick);
		if (that_client->get_username().size() > 0 && that_client->get_succesfully_reg() == false)
		{
			that_client->set_succesfully_reg(true);
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
	if (check_args_count(message, 4) == EXIT_FAILURE)
		return ;

	int	const & fd = message.get_fd();
	
	Client * that_client = get_client_obj(fd);
	if (that_client == NULL)
		return ;

	if (that_client->get_succesfully_reg() == false)
	{
		that_client->set_username(message.get_args().at(0));
		that_client->set_realname(message.get_postfix());
		if (that_client->get_nickname().size() > 0)
		{
			that_client->set_succesfully_reg(true);
			register_client(message);
		}
	}
	else
	{
		server_code_nick_text_message(fd, "462", "= " + message.get_args().at(0), "User already registered!");
	}
	
}


/* JOIN <channel>{,<channel>} [<key>{,<key>}] */
void	Server::JOIN(Message const & message)
{
	if (check_args_count(message, 1) == EXIT_FAILURE)
		return ;

	//@ToDo validity check for channel names

	int	const &			fd = message.get_fd();
	std::string	const &	channel_name = message.get_args().at(0);

	Client * that_client = get_client_obj(fd);
	if (that_client == NULL)
		return ;

	std::string nick = that_client->get_nickname();
	//:42.ft_irc.local 479 Rene channel :Illegal channel name
	if (check_channel_name(fd, channel_name) == EXIT_FAILURE)
		return ;

	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);
	if (channel == channel_list.end())
	{
		channel_list.insert(std::make_pair(channel_name, Channel(channel_name, fd)));
		channel = channel_list.find(channel_name);
	}

	if (check_nick_is_banned(fd, channel_name, nick) == EXIT_FAILURE)
		return ;
	
	if (check_invite_only(fd, channel_name, nick) == EXIT_FAILURE)
		return ;

	channel->second.add_member(fd, nick);

	nick_user_host_message(fd, message.get_cmd(), channel_name);
	nick_user_host_message(fd, message.get_cmd(), channel_name, channel_name);

	if (channel->second.get_topic().empty() == false)
	{
		server_code_text_message(fd, "332", nick + " " + channel_name, (channel->second.get_topic()));
	}
	
	server_code_text_message(fd, "MODE", (channel_name + " " + channel->second.get_channel_flags()) );
	NAMES(message);
}


/* PRIVMSG <msgtarget> :<text to be sent> */
void	Server::PRIVMSG(Message const & message)
{
	PRIVMSG_NOTICE(message, "PRIVMSG");
}


void	Server::PING(Message const & message)
{
	// if (check_args_count(message, 1) == EXIT_FAILURE)
	// 	return ;
	std::string	arg0;
	if(message.get_args().size() > 0)
		arg0 = message.get_args().at(0);
	server_code_server_text_message(message.get_fd(), "PONG", "", arg0);
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
	if (check_args_count(message, 1) == EXIT_FAILURE)
		return ;

	int const &			fd = message.get_fd();
	std::string	const &	channel_name = message.get_args().at(0);

	if (check_channel_exists(fd, channel_name) == EXIT_FAILURE)
		return ;
	
	if (check_client_in_channel(fd, channel_name) == EXIT_FAILURE)
		return ;

	std::map<std::string, Channel>::iterator	channel_it = channel_list.find(channel_name);
	channel_it->second.remove_member(fd);
	nick_user_host_message(fd, message.get_cmd() + " " + channel_name);

	//if channel is empty remove it from list
	if (channel_it->second.get_member_list().size() == 0)
	{
		channel_list.erase(channel_it);
		return ;
	}
	nick_user_host_message(fd, message.get_cmd() + " " + channel_name, "", channel_name);
}


void	Server::WHOIS(Message const & message)
{
	not_implemented_yes(message);
}


void	Server::WHO(Message const & message)
{
	int const & fd = message.get_fd();
	std::map<int, Client>::iterator client_it = client_list.begin();
	std::map<int, Client>::iterator client_ite = client_list.end();

	for (; client_it != client_ite; ++client_it)
	{
		Client const & that_client = client_it->second;
		server_code_nick_text_message(fd, "352", "* " + that_client.get_username() + " " + that_client.get_hostname() + " " + this->server_name + " H", "0 " + that_client.get_realname());
	}

	server_code_nick_text_message(fd, "315", "*", "End of /WHO list.");
}


void	Server::WHOWAS(Message const & message)
{
	not_implemented_yes(message);
}


void	Server::NAMES(Message const & message)
{
	if (check_args_count(message, 1) == EXIT_FAILURE)
		return ;

	std::string arg0;
	std::string list;
	if (message.get_args().empty() == false)
	{
		arg0 = message.get_args().at(0);
		list = channel_list.find(arg0)->second.get_member_string_operator();
	}
	server_code_nick_text_message(message.get_fd(), "353", "= " + arg0, list);
	server_code_nick_text_message(message.get_fd(), "366", arg0, "End of /NAMES list.");
}


void	Server::MOTD(Message const & message)
{
	int	const & fd = message.get_fd();
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
	// if (check_args_count(message, 1) == EXIT_FAILURE)
	// 	return ;
	int	const &		fd = message.get_fd();
	std::string	response = "bye bye";
	if (message.get_args().size() > 0)
		response = message.get_args().at(0);
	std::map<std::string, Channel>::iterator channel_it = channel_list.begin();
	std::map<std::string, Channel>::iterator channel_ite = channel_list.end();
	for (; channel_it != channel_ite; ++channel_it)
	{
		if (channel_it->second.is_client_on_channel(fd) == true)
		{
			channel_it->second.remove_member(fd);
			nick_user_host_message(fd, "QUIT", response, channel_it->first);
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
	if (check_args_count(message, 2) == EXIT_FAILURE)
		return ;

	int	const &			fd = message.get_fd();
	std::string	const &	nick_name = message.get_args().at(0);
	std::string	const &	channel_name = message.get_args().at(1);

	if (check_channel_exists(fd, channel_name) == EXIT_FAILURE)
		return ;

	if (check_client_in_channel(fd, channel_name) == EXIT_FAILURE)
		return ;

	if (check_nick_exists(fd, nick_name) == EXIT_FAILURE)
		return ;

	if (check_client_is_operator(fd, channel_name) == EXIT_FAILURE)
		return ;

	std::string	const &	sender_nick = client_list.find(fd)->second.get_nickname();
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);

	channel_it->second.add_invite(nick_name);
	nick_user_host_message(fd, "341 " + sender_nick + " " + nick_name + " " + channel_name);
	nick_user_host_message(get_client_fd(nick_name), message.get_cmd() + " " + nick_name, channel_name);
	//only to channel operators
	//nick_user_host_message(message.get_fd(), "341 " + sender_nick + " " + nick_name + " " + channel_name, channel_name);
}


/* KICK <channel> <user> [<comment>] */
void	Server::KICK(Message const & message)
{
	if (check_args_count(message, 2) == EXIT_FAILURE)
		return ;

	int	const &			fd = message.get_fd();
	std::string const &	channel_name = message.get_args().at(0);
	std::string const &	kicked_nick = message.get_args().at(1);

	if (check_channel_exists(fd, channel_name) == EXIT_FAILURE)
		return ;

	if (check_client_in_channel(fd, channel_name) == EXIT_FAILURE)
		return ;

	if (check_nick_exists(fd, kicked_nick) == EXIT_FAILURE)
		return ;

	if (check_nick_in_channel(fd, channel_name, kicked_nick) == EXIT_FAILURE)
		return ;

	if (check_client_is_operator(fd, channel_name) == EXIT_FAILURE)
		return ;

	std::string	reason;
	std::string	const &	sender_nick = client_list.find(fd)->second.get_nickname();
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);
	std::map<int, Client>::iterator client_it = get_client_by_nick(kicked_nick);
	
	channel_it->second.remove_member(client_it->first);
	if (message.get_args().size() > 2)
		reason = message.get_args().at(2);
	else
		reason = sender_nick;
	nick_user_host_message(fd, message.get_cmd() + " " + channel_name + " " + kicked_nick, reason);
	std::string tmp = ":" + get_nick_user_host_txt(fd) + " " + message.get_cmd() + " " + channel_name + " " +  kicked_nick + " :" + reason + "\r\n";
	send_message_queue.push(Message(client_it->first, tmp));
	//nick_user_host_message(client_it->first, message.get_cmd() + " " + channel_name + " " + kicked_nick, reason);
	nick_user_host_message(fd, message.get_cmd() + " " + channel_name + " " + kicked_nick, reason, channel_name);
}


void	Server::LIST(Message const & message)
{
	int const &	fd = message.get_fd();
	
	server_code_nick_text_message(fd, "321", "Channel", "Users Name");

	std::map<std::string, Channel>::iterator channel_it = channel_list.begin();
	std::map<std::string, Channel>::iterator channel_ite = channel_list.end();
	std::string number_string;
	for (; channel_it != channel_ite ; ++channel_it)
	{
		number_string = number_to_string(channel_it->second.get_member_list().size());
		server_code_nick_text_message(fd, "322", channel_it->first + " " + number_string, "[" + channel_it->second.get_channel_flags() + "]");
	}

	server_code_nick_text_message(fd, "323", "", "End of /LIST");
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
	if (check_args_count(message, 1) == EXIT_FAILURE)
		return ;

	int	const &			argc = message.get_args().size();
	int	const &			fd = message.get_fd();
	std::string	const &	channel_or_nick = message.get_args().at(0);
	
	if (check_channel_exists(fd, channel_or_nick) == EXIT_FAILURE)
		return ;

	if (check_client_in_channel(fd, channel_or_nick) == EXIT_FAILURE)
		return ;

	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_or_nick);

	if (argc > 1)
	{

		if (check_client_is_operator(fd, channel_or_nick) == EXIT_FAILURE)
			return ;
			
		std::string const &	flags = message.get_args().at(1);
		if (flags.find('-') != std::string::npos && flags.find('+') != std::string::npos)
			return ;

		if ( (flags.find('o') != std::string::npos) && (argc > 2) )
		{
			if (check_nick_exists(fd, message.get_args().at(2)) == EXIT_FAILURE)
				return ;

			std::string	const & nick_name = message.get_args().at(2);
			std::map<int, Client>::iterator client_it = get_client_by_nick(nick_name);

			if (check_nick_in_channel(fd, channel_or_nick, nick_name) == EXIT_FAILURE)
				return ;

			if (flags.find('-') != std::string::npos)
				channel_it->second.remove_operator(client_it->first);
			else
				channel_it->second.add_operator(client_it->first);

			nick_user_host_message(fd, message.get_cmd() + " " + channel_or_nick + " " + flags + " " + nick_name);
			nick_user_host_message(fd, message.get_cmd() + " " + channel_or_nick + " " + flags + " " + nick_name, "", channel_or_nick);
			//return ;
		}

		//:42.ft_irc.local 474 rene #42 :Cannot join channel (+b)
		if (flags.find('b') != std::string::npos && argc > 2)
		{
			if (check_nick_exists(fd, message.get_args().at(2)) == EXIT_FAILURE)
				return ;

			std::string	const &	nick_name = message.get_args().at(2);
			std::map<int, Client>::iterator client_it = get_client_by_nick(nick_name);

			if (flags.find('-') != std::string::npos)
				channel_it->second.remove_ban(client_it->second.get_nickname());
			else
				channel_it->second.add_ban(client_it->second.get_nickname());
				
			nick_user_host_message(fd, message.get_cmd() + " " + channel_or_nick + " " + flags + " " + nick_name);
			nick_user_host_message(fd, message.get_cmd() + " " + channel_or_nick + " " + flags + " " + nick_name, "", channel_or_nick);
		}
		if (flags.find('i') != std::string::npos)
		{
			if (flags.find('-') != std::string::npos)
				channel_it->second.set_channel_invite_only(false);
			else
				channel_it->second.set_channel_invite_only(true);

			nick_user_host_message(fd, message.get_cmd() + " " + channel_or_nick + " " + flags);
			nick_user_host_message(fd, message.get_cmd() + " " + channel_or_nick + " " + flags, "", channel_or_nick);
		}
		if (flags.find('t') != std::string::npos)
		{
			if (flags.find('-') != std::string::npos)
				channel_it->second.set_channel_topic_only(false);
			else
				channel_it->second.set_channel_topic_only(true);

			nick_user_host_message(fd, message.get_cmd() + " " + channel_or_nick + " " + flags);
			nick_user_host_message(fd, message.get_cmd() + " " + channel_or_nick + " " + flags, "", channel_or_nick);
		}
		if (flags.find('n') != std::string::npos)
		{
			if (flags.find('-') != std::string::npos)
				channel_it->second.set_channel_inside_only(false);
			else
				channel_it->second.set_channel_inside_only(true);

		}
			nick_user_host_message(fd, message.get_cmd() + " " + channel_or_nick + " " + flags);
			nick_user_host_message(fd, message.get_cmd() + " " + channel_or_nick + " " + flags, "", channel_or_nick);
		return ;
	}
	server_code_nick_text_message(fd, "324", (channel_or_nick + " " + channel_it->second.get_channel_flags()));
	server_code_nick_text_message(fd, "329", channel_or_nick);
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
	if (check_args_count(message, 2) == EXIT_FAILURE)
		return ;

	int	const &			fd = message.get_fd();
	std::string	const &	channel_or_nick = message.get_args().at(0);

	if (channel_or_nick[0] == '#')
	{
		if (check_channel_exists(message.get_fd(), channel_or_nick) == EXIT_FAILURE)
			return ;

		std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_or_nick);
		if (channel_it->second.is_channel_inside_only() == true && check_client_in_channel(fd, channel_or_nick) == EXIT_FAILURE)
			return ;
		
		nick_user_host_message(fd, type + " " +  channel_or_nick, message.get_postfix(), channel_or_nick);
		
		ircbot_msg(message);
		return ;
	}
	
	if (check_nick_exists(fd, channel_or_nick) == EXIT_FAILURE)
		return ;

	std::map<int, Client>::iterator		it_client = get_client_by_nick(channel_or_nick);
	std::string tmp = ":" + get_nick_user_host_txt(fd) + " " + type + " " +  channel_or_nick + " :" + message.get_postfix() + "\r\n";
	send_message_queue.push(Message(it_client->first, tmp));
}

void	Server::TOPIC(Message const & message)
{
	if (check_args_count(message, 1) == EXIT_FAILURE)
		return ;

	int	const &			fd = message.get_fd();
	std::string	const &	channel_name = message.get_args().at(0);
	
	if (check_channel_exists(fd, channel_name) == EXIT_FAILURE)
		return ;

	if (check_client_in_channel(fd, channel_name) == EXIT_FAILURE)
		return ;

	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);

	//if args > 1 then change topic
	if ( message.get_args().size() > 1 )
	{
		if (channel_it->second.is_channel_topic_only() == true)
			if (check_client_is_operator(fd, channel_name) == EXIT_FAILURE)
				return ;
		channel_it->second.set_topic(message.get_args().at(1));
		nick_user_host_message(fd, message.get_cmd() + " " + channel_name + " ", channel_it->second.get_topic());
		nick_user_host_message(fd, message.get_cmd() + " " + channel_name + " ", channel_it->second.get_topic(), channel_name);
		return ;
	}
	
	nick_user_host_message(fd, message.get_cmd() + " " + channel_name + " ", channel_it->second.get_topic());
}
