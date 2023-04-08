#pragma once

# include <unistd.h>
# include <algorithm>
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

		bool	findClientNick(std::string const nickname) const;
		void	sendAll(int fd, Client &client, std::string const message) const;
		void	addChannel(Channel *channel);
		void	eraseChannel(Channel *channel);
		void	run(void);

		const std::string 	getPass() const;
		Channel				*getChannel(std::string const &name) const;

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
