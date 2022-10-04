/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_process.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ehosu <ehosu@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 17:06:14 by rkaufman          #+#    #+#             */
/*   Updated: 2022/10/04 11:26:19 by ehosu            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::process_messages(void)
{
	int	const CMDS_SIZE = 33;
	bool		executed_cmd = false;
	std::string cmd;

	std::string const CMDS[CMDS_SIZE] = {"PASS", "NICK", "USER", "JOIN", "PRIVMSG", "PING", "AWAY", "PART", \
	"WHOIS", "WHO", "WHOWAS", "NAMES", "MOTD", "RULES", "LUSERS", "MAP", "QUIT", "VERSION", "STATS", \
	"LINKS", "ADMIN", "INVITE", "KICK", "LIST", "NOTICE", "KNOCK", "SETNAME", "MODE", "SILENCE", "PONG", "TOPIC", "ISON", "DIE"};

	void	(Server::*func[CMDS_SIZE])(Message const &) = \
	{&Server::PASS, &Server::NICK, &Server::USER, &Server::JOIN, &Server::PRIVMSG, &Server::PING, &Server::AWAY, &Server::PART, \
	&Server::WHOIS, &Server::WHO, &Server::WHOWAS, &Server::NAMES, &Server::MOTD, &Server::RULES, &Server::LUSERS, \
	&Server::MAP, &Server::QUIT, &Server::VERSION, &Server::STATS, &Server::LINKS, &Server::ADMIN, &Server::INVITE, \
	&Server::KICK, &Server::LIST, &Server::NOTICE, &Server::KNOCK, &Server::SETNAME, &Server::MODE, &Server::SILENCE, &Server::PONG, \
	&Server::TOPIC, &Server::ISON, &Server::DIE};

	for (int i = 0, end = received_message_queue.size(); i < end; ++i)
	{
		Message & process_message = received_message_queue.front();
		process_message.parse();
		#if (DEBUG)
			process_message.print_message();
		#endif
		executed_cmd = false;
		cmd = process_message.get_cmd();

		for (int ii = 0; ii < CMDS_SIZE; ++ii)
		{
			if (process_message.get_cmd() == CMDS[ii])
			{
				(this->*func[ii])(process_message);
				executed_cmd = true;
				break;
			}
		}

		if (executed_cmd == false)
			server_code_nick_text_message(process_message.get_fd(), "421", process_message.get_cmd(), "Unknown command!");

		received_message_queue.pop();
	}

	for (int i = 0, end = new_users_message_queue.size(); i < end; ++i)
	{
		Message & process_message = new_users_message_queue.front();
		process_message.parse();
		#if (DEBUG)
			process_message.print_message();
		#endif
		executed_cmd = false;
		cmd = process_message.get_cmd();

		if (cmd == "PASS" || check_authentication(process_message.get_fd()) == EXIT_SUCCESS)
		{
			for (int ii = 0; ii < 3; ++ii)
			{
				if (process_message.get_cmd() == CMDS[ii])
				{
					(this->*func[ii])(process_message);
					executed_cmd = true;
					break;
				}
			}

			if (executed_cmd == false)
				server_code_nick_text_message(process_message.get_fd(), "451", "*", "You have not registered");
		}
		new_users_message_queue.pop();
	}
}


/*<nick>!<user>@<host>*/
std::string Server::get_nick_user_host_txt(int const & fd)
{
	Client	& that_client = client_list.find(fd)->second;
	std::string nick = that_client.get_nickname();
	std::string user = that_client.get_username();
	std::string host = that_client.get_hostname();
	std::string tmp = nick + "!" + user + "@" + host;
	return (tmp);
}


/*:<server> <code> <nick> [ <text> ] [ :<postfix> ]*/
void	Server::server_code_nick_text_message(int const & fd, std::string const & code, std::string const & text, std::string const & postfix)
{
	std::string	nick = client_list.find(fd)->second.get_nickname();
	server_code_message(nick, fd, code, text, postfix);
}


/*:<server> <code> <server> [ <text> ] [ :<postfix> ]*/
void	Server::server_code_server_text_message(int const & fd, std::string const & code, std::string const & text, std::string const & postfix)
{
	server_code_message(server_name, fd, code, text, postfix);
}


/*:<server> <code> [ <text> ] [ :<postfix> ]*/
void	Server::server_code_text_message(int const & fd, std::string const & code, std::string const & text, std::string const & postfix)
{
	server_code_message("", fd, code, text, postfix);
}


void	Server::server_code_message(std::string const & nick_server, int const & fd, std::string const & code, std::string const & text, std::string const & postfix)
{
	std::string	nick_serv, fix, txt;
	if (nick_server.size() > 0)
		nick_serv = " " + nick_server;
	if (postfix.size() > 0)
		fix = " :" + postfix;
	if (text.size() > 0)
		txt = " " + text;
	std::string msg = ":" + server_name + " " + code + nick_serv + txt + fix + "\r\n";
	send_message_queue.push(Message(fd, msg));
}


/*:<nick>!<user>@<host> <code> [ :<postfix> ]*/
void Server::nick_user_host_message(int const & fd, std::string const & code, std::string const & postfix, std::string const & receiver)
{
	Client	that_client = client_list.find(fd)->second;
	std::string	fix;
	if (postfix.size() > 0)
		fix = " :" + postfix;
	std::string msg = ":" + get_nick_user_host_txt(fd) + " " + code + fix + "\r\n";
	send_message_queue.push(Message(fd, msg, receiver));
}


int Server::check_args_count(Message const & message, size_t const & args_count)
{
	if (message.get_args().size() < args_count)
	{
		server_code_nick_text_message(message.get_fd(), "461", message.get_cmd(), "Not enough parameters");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int	Server::check_authentication(int const & client_fd)
{
	std::map<int, Client>::iterator client_it = client_list.find(client_fd);

	if (client_it == client_list.end())
		return (EXIT_FAILURE);

	if (client_it->second.get_authenticated() == false)
	{
		server_code_nick_text_message(client_fd, "451", "*", "You have not registered");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int	Server::check_registration(int const & client_fd)
{
	std::map<int, Client>::iterator client_it = client_list.find(client_fd);

	if (client_it == client_list.end())
		return (EXIT_FAILURE);

	if (client_it->second.get_succesfully_reg() == false)
	{
		server_code_nick_text_message(client_fd, "451", "*", "You have not registered");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int Server::check_channel_exists(int const & fd, std::string const & channel_name)
{
	
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);

	if (channel_it == channel_list.end())
	{
		server_code_nick_text_message(fd, "402", channel_name, "No such channel");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int Server::check_client_in_channel(int const & fd, std::string const & channel_name)
{
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);
	
	if (!channel_it->second.is_client_on_channel(fd))
	{
		server_code_nick_text_message(fd, "442", channel_name, "You are not on that channel");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int Server::check_client_already_in_channel(int const & fd, std::string const & nick_name, std::string const & channel_name)
{
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);
	
	if (channel_it->second.is_client_on_channel(get_client_fd(nick_name)))
	{
		server_code_nick_text_message(fd, "443", nick_name + " " + channel_name, "Is already on channel");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int Server::check_nick_exists(int const & sender_fd, std::string const & check_nick)
{
	std::map<int, Client>::iterator client_it = get_client_by_nick(check_nick);

	if (client_it == client_list.end())
	{
		server_code_nick_text_message(sender_fd, "401", check_nick, "No such nick");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int Server::check_nick_in_channel(int const & fd, std::string const & channel_name, std::string const & nick_name)
{
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);
	
	if (channel_it->second.is_client_on_channel(get_client_fd(nick_name)) == false)
	{
		server_code_nick_text_message(fd, "441", nick_name, "Not in channel");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int Server::check_client_is_operator(int const & fd, std::string const & channel_name)
{
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);

	if (channel_it->second.is_client_is_operator(fd) == false)
	{
		std::string	sender_nick = client_list.find(fd)->second.get_nickname();
		server_code_nick_text_message(fd, "482", channel_name, "You are not channel operator");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int	Server::check_nick_is_banned(int const & fd, std::string const & channel_name, std::string const & nick_name)
{
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);
	
	if (channel_it->second.get_ban_list().find(nick_name) != channel_it->second.get_ban_list().end())
	{
		server_code_nick_text_message(fd, "474", channel_name, "Cannot join channel (+b)");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int	Server::check_channel_name(int const & fd, std::string const & channel_name)
{
	if (channel_name[0] != '#' && channel_name[0] != '&' && is_invalid_channel_name(channel_name) == true)
	{
		server_code_nick_text_message(fd, "479", channel_name, "Illegal channel name");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}


int	Server::check_invite_only(int const & fd, std::string const & channel_name, std::string const & nick)
{
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(channel_name);

	if (channel_it->second.is_channel_invite_only() == true && channel_it->second.get_invite_list().find(nick) == channel_it->second.get_invite_list().end())
	{
		server_code_nick_text_message(fd, "473", channel_name, "Cannot join channel (+i)");
		return (EXIT_FAILURE);
	}
	channel_it->second.remove_invite(nick);
	return (EXIT_SUCCESS);
}


bool	Server::is_invalid_channel_name(std::string const & channel_name)
{
	char const * channel_char = channel_name.c_str();

	for (; *channel_char != '\0'; ++channel_char)
	{
		if (std::iscntrl(*channel_char) != 0)
			return (true);
	}
	return (false);
}
