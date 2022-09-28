/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 16:15:31 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/28 17:33:46 by rkaufman         ###   ########.fr       */
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
		void				add_invite(std::string const & nick);
		void				remove_invite(std::string const & nick);
		bool				is_client_on_channel(int const & fd) const;
		bool				is_client_is_operator(int const & fd) const;
		
		bool				is_channel_invite_only(void) const;
		void				set_channel_invite_only(bool const & invite);

		void				set_channel_topic_only(bool const & topic);
		bool				is_channel_topic_only(void) const;
		
		void				set_channel_inside_only(bool const & inside);
		bool				is_channel_inside_only(void) const;
		
		void				set_topic(std::string const & input);

		std::string const	get_channel_flags(void) const;
		std::string const	get_member_string(void) const;
		std::string const	get_member_string_operator(void) const;
		std::map<int, std::string> const & get_member_list(void) const;
		std::set<std::string> const	& get_ban_list(void) const;
		std::set<std::string> const	& get_invite_list(void) const;
		std::string const &	get_topic(void) const;
		
	private:

		Channel const & operator=(Channel const & rhs);

		bool						invite_only;	//+i
		bool						topic_only;		//+t
		bool						inside_only;	//+n
		std::string					name;
		std::string					topic;
		std::set<int>				operator_list;
		std::map<int, std::string>	member_list;
		std::set<std::string>		ban_list;
		std::set<std::string>		invite_list;
		
};