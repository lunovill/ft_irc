#pragma once

# include <map>
# include <sys/socket.h>

# include "Client.hpp"

# define CLRF	"\r\n"

class	Channel {

	public:

		Channel(std::string name);
		Channel(std::string name, std::string password);
		~Channel(void);

		std::string	topic;

		bool		findClient(int fd) const;
		bool		findClient(std::string const &nickname) const;
		bool		addClient(int fd, Client &client);
		bool		eraseClient(int fd);
		void        eraseMode(char const &mode);
		void		sendAll(int const &senderFd, Client const &sender,std::string const &message, bool const &oper) const;
		std::string clientList(std::string const &firstName) const;

		std::string 	getName(void) const;
		std::string 	getPass(void) const;
		std::string 	getMode(void) const;
		void			setMode(char const &mode);
		void        	setPass(std::string &password);
		void        	setClientLimit(std::string &clientLimit);


	private:

		std::string const		_name;
		std::string				_password;
		std::string				_mode;
		size_t					_clientLimit;
		std::map<int, Client *> _clients;
};
