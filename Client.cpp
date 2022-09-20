/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ehosu <ehosu@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 16:13:58 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/20 15:44:27 by ehosu            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Client::Client(void) :fd(-1), nickname(""), username("")
// {}

Client::Client(int const & fd) :fd(fd), nickname(""), username(""), succesfully_reg(false)
{}

Client::Client(std::string const & nickname) :fd(-1), nickname(nickname), username(""), message_buffer(""), succesfully_reg(false)
{}

Client::Client(Client const & other) :fd(other.fd), nickname(other.nickname), username(other.username), message_buffer(""), succesfully_reg(false)
{}

Client & Client::operator=(Client const & rhs)
{
	this->~Client();
	new (this) Client(rhs);
	return (*this);
}

Client::~Client(void)
{}

void	Client::set_nickname(std::string const & nickname)
{
	this->nickname = nickname;
}

void	Client::set_realname(std::string const & realname)
{
	this->realname = realname;
}

void	Client::set_username(std::string const & username)
{
	this->username = username;
}

std::string const & Client::get_username(void) const
{
	return (this->username);
}

void	Client::set_succesfully_reg(bool const & succesfully_reg)
{
	this->succesfully_reg = succesfully_reg;
}

bool const & Client::get_succesfully_reg(void) const
{
	return (this->succesfully_reg);
}

void	Client::set_hostname(std::string const & hostname)
{
	this->hostname = hostname;
}

std::string const & Client::get_hostname(void) const
{
	return (this->hostname);
}

bool Client::operator<(Client const & rhs) const
{
	return (this->nickname < rhs.nickname);
}

std::string const & Client::get_nickname(void) const
{
	return (this->nickname);
}

void	Client::clear_message_buffer(void)
{
	this->message_buffer.clear();
}

void	Client::append_message_buffer(char const * input)
{
	this->message_buffer.append(input);
}

void	Client::append_message_buffer(std::string const & input)
{
	this->message_buffer.append(input);
}

std::string const & Client::get_message_buffer(void) const
{
	return (this->message_buffer);
}