/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 16:23:05 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/16 16:57:59 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string const & name, int const & fd) :name(name), moderator(fd)
{}

Channel::Channel(Channel const & other) :name(other.name), moderator(other.moderator)
{}

Channel const & Channel::operator=(Channel const & rhs)
{
	this->~Channel();
	new (this) Channel(rhs);
	return (*this);
}

Channel::~Channel(void)
{}

void	Channel::add_member(int const & fd)
{
	member_list.insert(fd);
}

void	Channel::remove_member(int const & fd)
{
	member_list.erase(fd);
}

std::set<int> const &	Channel::get_member_list(void) const
{
	return (this->member_list);
}
