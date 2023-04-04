#pragma once

# include "Channel.hpp"

# define CLRF						"\r\n"
# define SERVER_NAME				"ircserv"
# define SERVER_HOSTNAME			std::string(SERVER_NAME) + ".fr"
# define SERVER_VERSION				"v4.2"
# define CHANNEL_MODES				"iklnot"
# define USER_MODE					"io"
# define RPL_PREFIX(code, nick)		std::string(":" + SERVER_HOSTNAME + " " + code + " " + nick)

# define RPL_WELCOME(nick, user, host)			RPL_PREFIX("001", nick) + " :Welcome to the " + SERVER_NAME + " Network, " + nick + "!" + user + "@" + host + CLRF
# define RPL_YOURHOST(nick)						RPL_PREFIX("002", nick) + " :Your host is " + SERVER_HOSTNAME + ", running version " + SERVER_VERSION + CLRF
# define RPL_CREATED(nick, datetime)			RPL_PREFIX("003", nick) + " :This server was created " + datetime + CLRF
# define RPL_MYINFO(nick)						RPL_PREFIX("004", nick) + " " + SERVER_HOSTNAME + " " + SERVER_VERSION + " " + USER_MODES + " " + CHANNEL_MODES + CLRF

#define ERR_UNKNOWNCOMMAND(nick, command)			RPL_PREFIX("421", nick) + " :" + command + " :Unknown command" + CLRF

//	NICK
# define ERR_NONICKNAMEGIVEN(nick)				RPL_PREFIX("431", nick) + " :No nickname given" + CLRF
# define ERR_ERRONEUSNICKNAME(nick, errNick)	RPL_PREFIX("432", nick) + " " + errNick + " :Erroneous nickname" + CLRF
# define ERR_NICKNAMEINUSE(nick, errNick)		RPL_PREFIX("435", nick) + " " + errNick + " :Nickname is already in use" + CLRF 

// USER et PASS
#define ERR_NEEDMOREPARAMS(nick, cmd)			RPL_PREFIX("461", nick) + " :" + cmd + " :Not enough parameters" + CLRF
#define ERR_ALREADYREGISTRED(nick)				RPL_PREFIX("462", nick) + " :Unauthorized command (already registered)" + CLRF
// PASS
#define ERR_PASSWDMISMATCH(nick)				RPL_PREFIX("464", nick) + " :Password incorrect" + CLRF

class Server;

class	Irc {

	public:

		Irc(Server *server);
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

		Server		*_server;
};
