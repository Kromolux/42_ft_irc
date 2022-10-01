/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_ircbot.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 17:16:11 by rkaufman          #+#    #+#             */
/*   Updated: 2022/10/01 15:03:55 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::bot_populate_reply_map(void)
{
	{
		std::vector<std::string> & bot_response	= (this->bot_reply_map.insert( std::make_pair( "hello", std::vector<std::string>() ) ).first->second);
		bot_response.push_back("Hi how are you?");
		bot_response.push_back("Nice to be here with you.");
		bot_response.push_back("You can ask me almost everything, but I decide if I wanna answer you.");
		bot_response.push_back("You are repeating yourself!");
		bot_response.push_back("Try something else please.");
		bot_response.push_back("You are so boring.");
		bot_response.push_back("I am still here.");
	}
	{
		std::vector<std::string> & bot_response	= (this->bot_reply_map.insert( std::make_pair( "weather", std::vector<std::string>() ) ).first->second);
		bot_response.push_back("What do you think what I am? Go ask google!");
		bot_response.push_back("Take a look out of the window.");
		bot_response.push_back("Ask your peers.");
		bot_response.push_back("Rain and cloudy.");
		bot_response.push_back("Too cold for a programmer most propably.");
	}
	{
		std::vector<std::string> & bot_response	= (this->bot_reply_map.insert( std::make_pair( "joke", std::vector<std::string>() ) ).first->second);
		bot_response.push_back("A foo walks into a bar, takes a look around and says: \"Hello World!\".");
		bot_response.push_back("Sofware developers like to solve problems. If there are no problems handily available, they will create their on problems!");
		bot_response.push_back("Chuck Norris can take a screenshot of his blue screen!");
		bot_response.push_back("A programmer is a machine that turns coffee into code.");
		bot_response.push_back("A programmer is a person who fixed a problem that you don't know you have, in a way you don't understand.");
		bot_response.push_back("Algorithm is a word used by programmers when, they do not want to explain what they did.");
		bot_response.push_back("Hardware is the part of a computer you can kick!");
		bot_response.push_back("Teacher: What is your level of programming?\nStudent: Low\nTeacher: Ok fine you can write programs in assembler then.");
		bot_response.push_back("Why does C get all the girls, and Java gets none? – Because C doesn’t treat them as objects.");
		bot_response.push_back("C programmers never die. They are just <cast> into VOID.");
	}
	{
		std::vector<std::string> & bot_response	= (this->bot_reply_map.insert( std::make_pair( "time", std::vector<std::string>() ) ).first->second);
		bot_response.push_back("I have no watch. Maybe look at the taskbar.");
		bot_response.push_back("It is later then bevore.");
		bot_response.push_back("Time to go to sleep.");
	}
}

void	Server::bot_msg(Message const & message)
{
	std::string tmp = message.get_postfix();
	size_t start_pos = tmp.find("@ircbot");
	
	if (start_pos != std::string::npos)
	{
		std::string const & channel_name = message.get_args().at(0);
		std::string msg = ":ircbot!" + this->hostname + " PRIVMSG " + channel_name + " :Hi " + get_client_obj(message.get_fd())->get_nickname() + " I am here.\r\n";
		bool success = false;
		std::map<std::string, std::vector<std::string> >::const_iterator it = bot_reply_map.begin();
		std::map<std::string, std::vector<std::string> >::const_iterator ite = bot_reply_map.end();
		for (; it != ite; ++it)
		{
			if (tmp.find(it->first) != std::string::npos)
			{
				tmp = it->second.at(bot_reply % it->second.size());
				++bot_reply;
				success = true;
				break ;
			}
		}

		if (success == false)
			tmp = "I have no idea what you are talking about. Please talk to my creators Eduard and René!";
		msg = ":ircbot!" + this->hostname + " PRIVMSG " + channel_name + " :" + tmp + "\r\n";
		send_message_queue.push(Message(1, msg, channel_name));
	}
}

