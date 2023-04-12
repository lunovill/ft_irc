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

		// Verifie qu'un client appartient a la liste _clients, si il ne le toruve pas il renvoie false
		bool		findClient(int fd) const;
		bool		findClient(std::string const &nickname) const;
		// Verifie la limite (MODE +l) et add le client a la liste _clients, si il ne peut pas il renvoie false
		bool		addClient(int fd, Client &client);
		// Supprime de la liste un client, si c'etait le dernier du channel, elle renvoie false
		bool		eraseClient(int fd);
		// Renvoie la liste de tout les nicknames des clients sous la forme d'une seul string
		void        eraseMode(char const &mode);
		void		sendAll(int const &senderFd, Client const &sender,std::string const &message, bool const &oper) const;
		std::string clientList(std::string const &firstName) const;

		std::string 	getName(void) const;
		std::string 	getPass(void) const;
		std::string 	getMode(void) const;
		void			setMode(char const &mode);


	private:

		std::string const		_name;
		std::string	const		_password;
		std::string				_mode;
		size_t					_clientLimit;
		std::map<int, Client *> _clients;
};
