/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 16:15:31 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/20 23:30:33 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <set>
#include <map>
#include "Message.hpp"

class Channel
{
	public:
		Channel(std::string const & name, int const & fd);
		Channel(Channel const & other);
		Channel const & operator=(Channel const & rhs);
		~Channel(void);
		
		void	add_member(int const & fd, std::string const & nick);
		void	remove_member(int const & fd);
		std::string const	get_member_string(void);
		std::map<int, std::string> const & get_member_list(void) const;
		
	private:
		std::string	name;
		std::set<int>	moderator_list;
		std::map<int, std::string>	member_list;
		
};