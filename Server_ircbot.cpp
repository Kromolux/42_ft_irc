/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_ircbot.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 17:16:11 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/30 18:21:44 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::ircbot_populate_reply_map(void)
{
	this->ircbot_reply_map.insert( std::make_pair( "hello", std::vector<std::string>() ) );
	std::vector<std::string> & bot_response = this->ircbot_reply_map.find("hello")->second;
	bot_response.push_back("Hi <NICK> how are you?");
}

void	Server::ircbot_msg(Message const & message)
{
	std::string tmp = message.get_postfix();
	size_t start_pos = tmp.find("@ircbot");
	std::string	whitespace = " \t\n\v\f\r";
	size_t end_pos = 0;
	
	if (start_pos != std::string::npos)
	{
		std::string const & channel_name = message.get_args().at(0);
		std::string msg = ":ircbot!" + this->hostname + " PRIVMSG " + channel_name + " :Hi " + get_client_obj(message.get_fd())->get_nickname() + " I am here.\r\n";
		send_message_queue.push(Message(1, msg, channel_name));

		end_pos = tmp.find_first_of(whitespace, start_pos);
		if (end_pos == std::string::npos)
			return ;
		start_pos = tmp.find_first_not_of(whitespace, end_pos);
		if (start_pos == std::string::npos)
			return ;
		end_pos = tmp.find_first_of(whitespace, start_pos);
		
		std::string token = tmp.substr(start_pos, end_pos - start_pos);
		std::cout << "first string after @ircbot [" << token << "]\n";
		std::map<std::string, std::vector<std::string> >::iterator it = ircbot_reply_map.find(token);
		if (it != ircbot_reply_map.end())
		{
			tmp = it->second.at(0); //@ToDo randomize selection
			start_pos = tmp.find("<NICK>");
			tmp.erase(start_pos, 6);
			std::string const & nick_name = get_client_obj(message.get_fd())->get_nickname();
			//tmp.replace(start_pos, nick_name.length(), nick_name);
			tmp.insert(start_pos, nick_name);
			std::cout << "tmp " << tmp << "\n";
			msg = ":ircbot!" + this->hostname + " PRIVMSG " + channel_name + " :" + tmp + "\r\n";
			send_message_queue.push(Message(1, msg, channel_name));
		}
	}
}

