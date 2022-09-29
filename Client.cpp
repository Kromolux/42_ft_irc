/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 16:13:58 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/29 11:25:33 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int const & fd) :fd(fd), nickname(""), username(""), authenticated(false), succesfully_reg(false)
{}

Client::Client(Client const & other)
{
	this->fd = other.fd;
	this->nickname = other.nickname;
	this->username = other.username;
	this->realname = other.realname;
	this->hostname = other.hostname;
	this->message_buffer = other.message_buffer;
	this->authenticated = other.authenticated;
	this->succesfully_reg = other.succesfully_reg;
}

Client & Client::operator=(Client const & rhs)
{
	this->~Client();
	new (this) Client(rhs);
	return (*this);
}

Client::~Client(void)
{}


bool Client::operator<(Client const & rhs) const
{ return (this->nickname < rhs.nickname); }


void	Client::set_nickname(std::string const & nickname)
{ this->nickname = nickname; }

std::string const & Client::get_nickname(void) const
{ return (this->nickname); }


void	Client::set_username(std::string const & username)
{ this->username = username; }

std::string const & Client::get_username(void) const
{ return (this->username); }


void	Client::set_realname(std::string const & realname)
{ this->realname = realname; }

std::string const & Client::get_realname(void) const
{ return (this->realname); }


void	Client::set_hostname(std::string const & hostname)
{ this->hostname = hostname; }

std::string const & Client::get_hostname(void) const
{ return (this->hostname); }


void	Client::clear_message_buffer(void)
{ this->message_buffer.clear(); }


void	Client::append_message_buffer(std::string const & input)
{ this->message_buffer.append(input); }

std::string const & Client::get_message_buffer(void) const
{ return (this->message_buffer); }


void	Client::set_authenticated(bool const & authenticated)
{ this->authenticated = authenticated; }

bool const & Client::get_authenticated(void) const
{ return (this->authenticated); }


void	Client::set_succesfully_reg(bool const & succesfully_reg)
{ this->succesfully_reg = succesfully_reg; }

bool const & Client::get_succesfully_reg(void) const
{ return (this->succesfully_reg); }
