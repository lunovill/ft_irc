#pragma once

# include <algorithm>
# include <string>
# include <unistd.h>
# include <vector>

# include "Irc.hpp"
# include "Socket.hpp"

# define BUFFER_SIZE 512

class Irc;

class	Server {

	public:

		Server(int port, std::string const password);
		~Server(void);

		// Verifie si le nick d'un client est deja utilise
		bool	findClientNick(std::string const &nickname) const;
		void	addChannel(Channel *channel);
		void	eraseChannel(Channel *channel);
		void	sendClient(int const &senderFd, Client const &sender, std::string const &recever, std::string const &message) const;
		void	sendAll(int const &senderFd, Client const &sender, std::string const &message) const;
		void	run(void);

		std::string const	getPass() const;
		Channel				*getChannel(std::string const &name) const;

	private:

		int						_port;
		std::string const		_password;
		std::string const		_mode;
		Socket					_socket;
		fd_set					_readFds;
		Irc						_command;
		std::map<int const, Client *>	_clients;
		std::vector<Channel*>	_channels;

		void	_acceptClient(int &clientFd) const;
		void	_commandRun(std::map<int const, Client *>::iterator &client, std::vector<std::string>  &inputs);
		void	_dataRecv(void);

};

// Voir si on le met dans un fichier Utils.{hpp/cpp}
std::string const	getTime();
