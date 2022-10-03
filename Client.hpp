/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 16:14:11 by rkaufman          #+#    #+#             */
/*   Updated: 2022/10/03 15:51:11 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

class Client
{
	public:

		Client(Client const & other);
		Client(int const & fd);
		~Client(void);
		
		bool 		operator<(const Client & rhs) const;
		
		void				set_nickname(std::string const & nickname);
		std::string const &	get_nickname(void) const;
		void				set_username(std::string const & username);
		std::string const &	get_username(void) const;
		void				set_realname(std::string const & realname);
		std::string const &	get_realname(void) const;
		void				set_hostname(std::string const & hostname);
		std::string const &	get_hostname(void) const;
		void				clear_message_buffer(void);
		void				append_message_buffer(std::string const & input);
		std::string const &	get_message_buffer(void) const;
	
		void				set_authenticated(bool const & authenticated);
		bool const &		get_authenticated(void) const;

		void				set_succesfully_reg(bool const & succesfully_reg);
		bool const &		get_succesfully_reg(void) const;

	
	private:

		Client & operator=(Client const & rhs);

		int	fd;
		std::string	nickname;
		std::string	username;
		std::string	realname;
		std::string	hostname;
		std::string	message_buffer;
		bool		authenticated;
		bool		succesfully_reg;
};
