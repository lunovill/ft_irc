#pragma once

# include <iostream>

# include "Client.hpp"

# define CLRF					"\r\n"
# define SERVER_HOSTNAME		std::string(SERVER_NAME) + ".fr"
# define SERVER_VERSION			"v4.2"
# define CHANNEL_MODES			"iklnot"
# define USER_MODE				"io"
# define RPL_PREFIX(code, nick)	std::string(":" + SERVER_HOSTNAME + " " + code + " " + nick)

# define RPL_WELCOME(nick, user, host)	RPL_PREFIX("001", nick) + " :Welcome to the " + SERVER_NAME + " Network, " + nick + "!" + user + "@" + host + CLRF
# define RPL_YOURHOST(nick)				RPL_PREFIX("002", nick) + " :Your host is " + SERVER_HOSTNAME + ", running version " + SERVER_VERSION + CLRF
# define RPL_CREATED(nick, datetime)	RPL_PREFIX("003", nick) + " :This server was created " + datetime + CLRF
# define RPL_MYINFO(nick)				RPL_PREFIX("004", nick) + " " + SERVER_HOSTNAME + " " + SERVER_VERSION + " " + USER_MODES + " " + CHANNEL_MODES + CLRF

class	Irc {

	public:

		Irc(void);
		~Irc(void);

		typedef void (Irc::*CommandFt)(int, Client &);

		struct commands {
			std::string name;
			CommandFt	cmd;
		};
		static const commands cmdList[];

		CommandFt	find(std::string const &input) const;
		void		NICK(int fd, Client &client);
		void		PASS(int fd, Client &client);
		void		USER(int fd, Client &client);

};
