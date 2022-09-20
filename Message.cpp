/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 10:53:12 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/20 21:28:09 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

Message::Message(int const & fd, std::string const & input, std::string const & receiver)
{
	this->fd = fd;
	this->raw = input;
	this->receiver = receiver;
}

Message::Message(int const & fd, char const * input)
{
	this->fd = fd;
	this->raw = input;
	this->complete = false;
}

Message::Message(Message const & other)
{
	this->CMD = other.CMD;
	this->sender = other.sender;
	this->receiver = other.receiver;
	this->content = other.content;
	this->raw = other.raw;
	this->fd = other.fd;
	this->complete = other.complete;
}

Message const & Message::operator=(Message const & rhs)
{
	this->~Message();
	new (this) Message(rhs);
	return (*this);
}

Message::~Message(void)
{}

void	Message::parse(void)
{
	std::string	tmp = this->raw;
	std::string	whitespace = " \t\n\v\f\r";
	size_t start_pos = 0, end_pos = 0, diff = 0;

	start_pos = tmp.find_first_not_of(whitespace, end_pos);
	if (start_pos == std::string::npos)
		return ; //empty message!
	end_pos = tmp.find_first_of(whitespace, start_pos);
	if (tmp[start_pos] == ':')
	{
		++start_pos;
		diff = end_pos - start_pos;
		this->prefix = tmp.substr(start_pos, diff);
		start_pos = tmp.find_first_not_of(whitespace, end_pos);
		end_pos = tmp.find_first_of(whitespace, start_pos);
	}
	if (start_pos == std::string::npos)
		return ;
	diff = end_pos - start_pos;
	this->CMD = tmp.substr(start_pos, diff);
	start_pos = tmp.find_first_not_of(whitespace, end_pos);
	end_pos = tmp.find_first_of(whitespace, start_pos);
	if (start_pos == std::string::npos)
		return ;
	diff = end_pos - start_pos;
	this->arg = tmp.substr(start_pos, diff);
	start_pos = tmp.find_first_not_of(whitespace, end_pos);
	end_pos = tmp.find_first_of(whitespace, start_pos);
	if (tmp[start_pos] == ':')
	{
		++start_pos;
		diff = end_pos - start_pos;
		this->postfix = tmp.substr(start_pos);
	}
	// if (tmp[0] == ':')
	// {
	// 	end_pos = tmp.find(' ', 0);
	// 	this->prefix = tmp.substr(0, end_pos);
	// 	//std::cout << "prefix: " << this->prefix << "\n";
	// 	tmp.erase(0, end_pos + 1);
	// 	//std::cout << "tmp: [" << tmp << "]\n";
	// }
	// //std::cout << "tmp: [" << tmp << "]\n";
	// //start_pos = tmp.find(' ');
	// end_pos = tmp.find(' ', 0);
	// this->CMD = tmp.substr(0, end_pos);
	// //std::cout << "CMD: " << this->CMD << "\n";
	// tmp.erase(0, end_pos + 1);
	// //std::cout << "tmp: [" << tmp << "]\n";
	// start_pos = tmp.rfind(" :");
	// //std::cout << "start_pos: " << start_pos << "\n";
	// if (start_pos != 0 && start_pos <= tmp.length())
	// {
	// 	//end_pos = raw.rfind(" ");
	// 	this->postfix = tmp.substr(start_pos + 1);
	// 	//std::cout << "postfix: " << this->postfix << "\n";
	// 	tmp.erase(start_pos);
	// 	//std::cout << "tmp: [" << tmp << "]\n";
	// }
	// this->arg = tmp;
}

std::string const & Message::get_cmd(void) const
{
	return (this->CMD);
}

std::string const & Message::get_arg(void) const
{
	return (this->arg);
}

std::string const & Message::print_message(void) const
{
	std::cout	<< "fd: [" << this->fd << "]\n"
				<< "prefix: [" << this->prefix << "]\n"
				<< "CMD: [" << this->CMD << "]\n"
				<< "arg: [" << this->arg << "]\n"
				<< "postfix: [" << this->postfix << "]\n";
	//return (this->CMD + " " + this->sender + " " + this->receiver + " " + this->content + "\r\n");
	return (this->raw);
}

int const & Message::get_fd(void) const
{
	return (this->fd);
}

int Message::get_len(void) const
{
	return (this->raw.length());
}

char const * Message::get_raw(void) const
{
	return (this->raw.c_str());
}

std::string const & Message::get_postfix(void) const
{
	return (this->postfix);
}

std::string const & Message::get_receiver(void) const
{
	return (this->receiver);
}

bool	Message::receiver_is_channel(void) const
{
	if (receiver[0] == '#')
		return (true);
	return (false);
}
