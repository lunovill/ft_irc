#pragma once

# include <unistd.h>
# include <vector>

# include "Irc.hpp"
# include "Socket.hpp"

# define BUFFER_SIZE 512

class Irc;

class	Server {

	public:

		Server(int port, std::string password);
		~Server(void);

		std::vector<Channel *>::iterator	addChannel(Channel &channel); // a fare
		void	run(void);

		std::map<int, Client *>	getClients(void) const;
		std::vector<Channel *> getChannel(void) const; // a faire
		const std::string getPass() const;

	private:

		int			_port;
		const std::string	_password;
		Socket		_socket;
		fd_set		_readFds;
		Irc			_command;
		std::map<int, Client *>	_clients;
		std::vector<Channel*>	_channels;

		void	_acceptClient(int &clientFd) const;
		void	_commandRun(std::map<int, Client *>::iterator &client, std::vector<std::string>  &inputs);
		void	_dataRecv(void);

};


// Voir si on le met dans un fichier Utils.{hpp/cpp}

#include <sstream>

template<typename T>
std::vector<std::string> to_split(T msg, char c = ' ')
{

	std::string line;
	std::vector<std::string> vec;
	std::istringstream stream(msg);

	while (std::getline(stream, line, c))
		if (!line.empty())
			vec.push_back(line);
	return vec;
}

std::string const	getTime();
