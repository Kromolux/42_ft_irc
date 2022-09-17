/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 16:15:31 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/16 16:57:09 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <set>
#include "Message.hpp"

class Channel
{
	public:
		Channel(std::string const & name, int const & fd);
		Channel(Channel const & other);
		Channel const & operator=(Channel const & rhs);
		~Channel(void);
		
		void	add_member(int const & fd);
		void	remove_member(int const & fd);
		std::set<int> const &	get_member_list(void) const;
		
	private:
		std::string	name;
		int	moderator;
		std::set<int> member_list;
		
};