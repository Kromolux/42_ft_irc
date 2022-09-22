/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_process.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 17:06:14 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/22 11:15:41 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::process_messages(void)
{
	const int	CMDS_SIZE = 30;
	bool		executed_cmd = false;

	std::string const CMDS[CMDS_SIZE] = {"PASS", "NICK", "USER", "JOIN", "PRIVMSG", "PING", "AWAY", "PART", \
	"WHOIS", "WHO", "WHOWAS", "NAMES", "MOTD", "RULES", "LUSERS", "MAP", "QUIT", "VERSION", "STATS", \
	"LINKS", "ADMIN", "INVITE", "KICK", "LIST", "NOTICE", "KNOCK", "SETNAME", "MODE", "SILENCE", "PONG"};

	void	(Server::*func[CMDS_SIZE])(Message const &) = \
	{&Server::PASS, &Server::NICK, &Server::USER, &Server::JOIN, &Server::PRIVMSG, &Server::PING, &Server::AWAY, &Server::PART, \
	&Server::WHOIS, &Server::WHO, &Server::WHOWAS, &Server::NAMES, &Server::MOTD, &Server::RULES, &Server::LUSERS, \
	&Server::MAP, &Server::QUIT, &Server::VERSION, &Server::STATS, &Server::LINKS, &Server::ADMIN, &Server::INVITE, \
	&Server::KICK, &Server::LIST, &Server::NOTICE, &Server::KNOCK, &Server::SETNAME, &Server::MODE, &Server::SILENCE, &Server::PONG};

	for (int i = 0, end = received_message_queue.size(); i < end; ++i)
	{
		Message & process_message = received_message_queue.front();
		process_message.parse();
		process_message.print_message();
		executed_cmd = false;

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
}

/*<nick>!<user>@<host>*/
std::string Server::get_nick_user_host_txt(int const & fd)
{
	Client	that_client = client_list.find(fd)->second;
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

//:user42!user42@i.love.debian.org MODE user42 :+i
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

int Server::check_channel(Message const & message)
{
	int fd = message.get_fd();
	std::map<std::string, Channel>::iterator channel_it = channel_list.find(message.get_arg());

	if (channel_it == channel_list.end())
	{
		server_code_nick_text_message(fd, "402", message.get_arg(), "No such channel");
		return (EXIT_FAILURE);
	}
	if (!channel_it->second.is_client_on_channel(fd))
	{
		server_code_nick_text_message(fd, "442", message.get_arg(), "You are not on that channel");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

int Server::check_client(Message const & message)
{
	std::map<int, Client>::iterator client_it = get_client(message.get_arg());

	if (client_it == client_list.end())
	{
		server_code_nick_text_message(message.get_fd(), "401", message.get_arg(), "No such nick");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
