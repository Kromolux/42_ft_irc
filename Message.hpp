/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkaufman <rkaufman@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/16 10:49:20 by rkaufman          #+#    #+#             */
/*   Updated: 2022/09/29 18:35:33 by rkaufman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include "colors.h"

class Message
{
	public:

		Message(int const & fd, char const * input);
		Message(int const & fd, std::string const & input, std::string const & receiver = "");
		Message(Message const & other);
		~Message(void);

		void	parse(void);
		
		int const & 		get_fd(void) const;
		std::string const &	get_prefix(void) const;
		std::string const &	get_cmd(void) const;
		std::string const &	get_arg(void) const;
		std::vector<std::string> const & get_args(void) const;
		std::string const &	get_postfix(void) const;
		std::string const &	get_receiver(void) const;
		char const *		get_raw(void) const;
		int					get_len(void) const;
		
		bool				receiver_is_channel(void) const;

		std::string const &	print_message(void) const;

	private:

		Message const & operator=(Message const & rhs);
		int			fd;
		std::string	prefix;
		std::string	cmd;
		std::vector<std::string>	args;
		std::string	sender;
		std::string	receiver;
		std::string	raw;
};