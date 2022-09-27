/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 16:15:31 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/27 18:58:10 by rkaufman         ###   ########.fr       */
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
		~Channel(void);
		
		void				add_member(int const & fd, std::string const & nick);
		void				remove_member(int const & fd);
		void				add_operator(int const & fd);
		void				remove_operator(int const & fd);
		void				add_ban(std::string const & nick);
		void				remove_ban(std::string const & nick);
		bool				is_client_on_channel(int const & fd);
		bool				is_client_is_moderator(int const & fd);
		
		std::string const	get_member_string(void);
		std::string const	get_member_string_moderator(void);
		std::map<int, std::string> const & get_member_list(void) const;
		std::set<std::string> const	& get_ban_list(void);
		void				set_topic(std::string const & input);
		std::string const &	get_topic(void) const;
		
	private:

		Channel const & operator=(Channel const & rhs);

		std::string					name;
		std::string					topic;
		std::set<int>				moderator_list;
		std::map<int, std::string>	member_list;
		std::set<std::string>		ban_list;
		
};