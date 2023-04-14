#pragma once

# include <algorithm>
# include <csignal>
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

		bool	findClient(std::string const &nickname) const;
		void	addChannel(Channel *channel);
		void	eraseChannel(Channel *channel);
		void	sendClient(Client const &sender, std::string const &recever, std::string const &message, bool const &oper) const;
		void	sendAll(int const &senderFd, Client const &sender, std::string const &message, bool const &oper) const;
		void	desconnectClient(std::string const &nickname);
		void	run(void);

		std::string const	getPass() const;
		Channel				*getChannel(std::string const &name) const;

	private:

		std::string const				_password;
		std::string const				_mode;
		Socket							_socket;
		static bool						_running;
		fd_set							_readFds;
		Irc								_command;
		std::map<int const, Client *>	_clients;
		std::vector<Channel*>			_channels;

		static void	_signalHandler(int signal);
		void	_acceptClient(int &clientFd) const;
		void	_commandRun(std::map<int const, Client *>::iterator &client, std::vector<std::string>  &inputs);
		void	_dataRecv(void);

};

// Voir si on le met dans un fichier Utils.{hpp/cpp}
std::string const	getTime();
