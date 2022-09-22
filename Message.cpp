/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 10:53:12 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/22 21:18:16 by rkaufman         ###   ########.fr       */
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
}

Message::Message(Message const & other)
{
	this->fd = other.fd;
	this->prefix = other.prefix;
	this->cmd = other.cmd;
	this->sender = other.sender;
	this->receiver = other.receiver;
	this->raw = other.raw;
	this->args = other.args;
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
	size_t start_pos = 0, end_pos = 0;

	start_pos = tmp.find_first_not_of(whitespace, 0);
	end_pos = tmp.find_first_of(whitespace, start_pos);
	if (start_pos == std::string::npos)
		return ; //empty message!
	if (tmp[start_pos] == ':')			//prefix
	{
		++start_pos;
		this->prefix = tmp.substr(start_pos, end_pos - start_pos);
	}
	else								//cmd
		this->cmd = tmp.substr(start_pos, end_pos - start_pos);
	tmp.erase(0, end_pos);

	end_pos = 0;
	do									//args
	{
		start_pos = tmp.find_first_not_of(whitespace, end_pos);
		end_pos = tmp.find_first_of(whitespace, start_pos);
		if (start_pos != std::string::npos)
		{
			if (tmp[start_pos] == ':')			//postfix
			{
				args.push_back(tmp.substr(start_pos + 1));
				return ;
			}
			args.push_back(tmp.substr(start_pos, end_pos - start_pos));
		}
	} while (start_pos != std::string::npos);
}

int const & Message::get_fd(void) const
{ return (this->fd); }

std::string const & Message::get_prefix(void) const
{ return (this->prefix); }

std::string const & Message::get_cmd(void) const
{ return (this->cmd); }

// std::string const & Message::get_arg(void) const
// { return (this->arg); }

std::vector<std::string> const & Message::get_args(void) const
{ return (this->args); }

// std::string const & Message::get_postfix(void) const
// { return (this->postfix); }

std::string const & Message::get_postfix(void) const
{ return (this->args.back()); }


std::string const & Message::get_receiver(void) const
{ return (this->receiver); }

char const * Message::get_raw(void) const
{ return (this->raw.c_str()); }

int Message::get_len(void) const
{ return (this->raw.length()); }

bool	Message::receiver_is_channel(void) const
{
	if (receiver[0] == '#')
		return (true);
	return (false);
}

std::string const & Message::print_message(void) const
{
	std::cout	<< "fd: [" << this->fd << "]\n"
				<< "prefix: [" << this->prefix << "]\n"
				<< "cmd: [" << this->cmd << "]\n";
	for (int i = 0, end = this->args.size(); i < end; ++i)
		std::cout << "args_" << i << " [" << this->args.at(i) << "]\n";
	return (this->raw);
}
