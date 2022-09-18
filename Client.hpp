/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 16:14:11 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/18 21:03:34 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

class Client
{
	public:
		//Client(void);
		Client(int const & fd);
		Client(std::string const & nickname);
		Client(Client const & other);
		Client & operator=(Client const & rhs);
		~Client(void);
		
		bool 		operator<(const Client & rhs) const;
		
		void		set_nickname(std::string const & nickname);
		std::string const & get_nickname(void) const;
		void		set_realname(std::string const & realname);
		void		set_username(std::string const & username);
		std::string const & get_username(void) const;
		void		set_hostname(std::string const & hostname);
		std::string const & get_hostname(void) const;
		void		clear_message_buffer(void);
		void		append_message_buffer(char const * input);
		void		append_message_buffer(std::string const & input);
		std::string const & get_message_buffer(void) const;
	
	private:
		int	fd;
		std::string	nickname;
		std::string	username;
		std::string	realname;
		std::string	hostname;
		std::string	message_buffer;
};