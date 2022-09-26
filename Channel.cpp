/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ehosu <ehosu@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 16:23:05 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/26 16:57:37 by ehosu            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string const & name, int const & fd) :name(name)
{
	moderator_list.insert(fd);
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
