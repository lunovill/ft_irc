#pragma once

# include <unistd.h>
# include <vector>
# include <string>

# include "Irc.hpp"
# include "Socket.hpp"

# define BUFFER_SIZE 512

class Irc;

class	Server {

	public:

		Server(int port, std::string password);
		~Server(void);

		std::vector<Channel *>::iterator	addChannel(Channel *channel);
		void	run(void);

		const std::string 		getPass() const;
		std::map<int, Client *>	getClients(void) const;
		std::vector<Channel *>	getChannels(void) const;

	private:

		int						_port;
		const std::string		_password;
		const std::string		_mode;
		Socket					_socket;
		fd_set					_readFds;
		Irc						_command;
		std::map<int, Client *>	_clients;
		std::vector<Channel*>	_channels;

		void	_acceptClient(int &clientFd) const;
		void	_commandRun(std::map<int, Client *>::iterator &client, std::vector<std::string>  &inputs);
		void	_dataRecv(void);

};


// Voir si on le met dans un fichier Utils.{hpp/cpp}

std::string const	getTime();
