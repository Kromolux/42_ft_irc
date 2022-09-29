/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 16:23:05 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/29 13:27:49 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string const & name, int const & fd) :name(name)
{
	operator_list.insert(fd);
	this->invite_only = false;
	this->topic_only = true;
	this->inside_only = true;
}

Channel::Channel(Channel const & other) :name(other.name)
{
	this->invite_only = other.invite_only;
	this->topic_only = other.topic_only;
	this->inside_only = other.inside_only;
	this->name = other.name;
	this->topic = other.topic;
	this->operator_list = other.operator_list;
	this->member_list = other.member_list;
	this->ban_list = other.ban_list;
	this->invite_list = other.invite_list;
}

Channel const & Channel::operator=(Channel const & rhs)
{
	this->~Channel();
	new (this) Channel(rhs);
	return (*this);
}

Channel::~Channel(void)
{}


void	Channel::add_member(int const & fd, std::string const & nick)
{
	if (member_list.size() == 0)
	{
		operator_list.insert(fd);
	}
	member_list.insert(std::make_pair(fd, nick));
}

void	Channel::remove_member(int const & fd)
{
	member_list.erase(fd);
	operator_list.erase(fd);
}

void	Channel::add_operator(int const & fd)
{
	operator_list.insert(fd);
}

void	Channel::remove_operator(int const & fd)
{
	operator_list.erase(fd);
}

void	Channel::add_ban(std::string const & nick)
{
	ban_list.insert(nick);
}

void	Channel::remove_ban(std::string const & nick)
{
	ban_list.erase(nick);
}
		
bool	Channel::is_client_on_channel(int const & fd) const
{
	std::map<int, std::string>::const_iterator it = member_list.find(fd);
	if (it == member_list.end())
		return (false);
	return (true);
}

bool	Channel::is_client_is_operator(int const & fd) const
{
	std::set<int>::const_iterator it = operator_list.find(fd);
	if (it == operator_list.end())
		return (false);
	return (true);
}

std::string const	Channel::get_member_string(void) const
{
	std::string tmp;
	std::map<int, std::string>::const_iterator it = member_list.begin();
	std::map<int, std::string>::const_iterator ite = member_list.end();
	for (; it != ite; ++it)
		tmp = tmp + it->second + " ";
	return (tmp);
}

std::string const	Channel::get_member_string_operator(void) const
{
	std::string tmp;
	std::map<int, std::string>::const_iterator it = member_list.begin();
	std::map<int, std::string>::const_iterator ite = member_list.end();
	for (; it != ite; ++it)
	{
		if (operator_list.find(it->first) == operator_list.end())
			tmp = tmp + it->second + " ";
		else
			tmp = tmp + "@" + it->second + " ";
	}
	return (tmp);
}

void				Channel::set_topic(std::string const & input)
{ this->topic = input; }

std::string const &	Channel::get_topic(void) const
{ return (this->topic); }


std::map<int, std::string> const & Channel::get_member_list(void) const
{ return (this->member_list); }

std::set<std::string> const	& Channel::get_ban_list(void) const
{ return (this->ban_list); }

void	Channel::set_channel_invite_only(bool const & invite)
{ this->invite_only = invite; }

bool	Channel::is_channel_invite_only(void) const
{ return (this->invite_only); }

std::set<std::string> const	& Channel::get_invite_list(void) const
{ return (this->invite_list); }

void	Channel::add_invite(std::string const & nick)
{ this->invite_list.insert(nick); }

void	Channel::remove_invite(std::string const & nick)
{ this->invite_list.erase(nick); }

void	Channel::set_channel_topic_only(bool const & topic)
{ this->topic_only = topic; }

bool	Channel::is_channel_topic_only(void) const
{ return (this->topic_only); }

void	Channel::set_channel_inside_only(bool const & inside)
{ this->inside_only = inside; }

bool	Channel::is_channel_inside_only(void) const
{ return (this->inside_only); }

//itnb
std::string const Channel::get_channel_flags(void) const
{
	std::string tmp;

	if (this->topic_only || this->inside_only || this->invite_only)
		tmp += "+";
	if (this->invite_only)
		tmp += "i";
	if (this->topic_only)
		tmp += "t";
	if (this->inside_only)
		tmp += "n";
	return (tmp);
}
