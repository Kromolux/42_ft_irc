/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 16:23:05 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/28 13:41:18 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string const & name, int const & fd) :name(name)
{
	moderator_list.insert(fd);
	this->invite_only = false;
}

Channel::Channel(Channel const & other) :name(other.name)
{}

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
		moderator_list.insert(fd);
	}
	member_list.insert(std::make_pair(fd, nick));
}

void	Channel::remove_member(int const & fd)
{
	member_list.erase(fd);
	moderator_list.erase(fd);
}

void	Channel::add_operator(int const & fd)
{
	moderator_list.insert(fd);
}

void	Channel::remove_operator(int const & fd)
{
	moderator_list.erase(fd);
}

void	Channel::add_ban(std::string const & nick)
{
	ban_list.insert(nick);
}

void	Channel::remove_ban(std::string const & nick)
{
	ban_list.erase(nick);
}
		
bool	Channel::is_client_on_channel(int const & fd)
{
	std::map<int, std::string>::iterator it = member_list.find(fd);
	if (it == member_list.end())
		return (false);
	return (true);
}

bool	Channel::is_client_is_moderator(int const & fd)
{
	std::set<int>::iterator it = moderator_list.find(fd);
	if (it == moderator_list.end())
		return (false);
	return (true);
}

std::string const	Channel::get_member_string(void)
{
	std::string tmp;
	std::map<int, std::string>::iterator it = member_list.begin();
	std::map<int, std::string>::iterator ite = member_list.end();
	for (; it != ite; ++it)
		tmp = tmp + it->second + " ";
	return (tmp);
}

std::string const	Channel::get_member_string_moderator(void)
{
	std::string tmp;
	std::map<int, std::string>::iterator it = member_list.begin();
	std::map<int, std::string>::iterator ite = member_list.end();
	for (; it != ite; ++it)
	{
		if (moderator_list.find(it->first) == moderator_list.end())
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

std::set<std::string> const	& Channel::get_ban_list(void)
{ return (this->ban_list); }

void	Channel::set_channel_invite_only(bool const & invite)
{ this->invite_only = invite; }

bool	Channel::is_channel_invite_only(void)
{ return (this->invite_only); }

std::set<std::string> const	& Channel::get_invite_list(void)
{ return (this->invite_list); }

void	Channel::add_invite(std::string const & nick)
{
	this->invite_list.insert(nick);
}

void	Channel::remove_invite(std::string const & nick)
{
	this->invite_list.erase(nick);
}