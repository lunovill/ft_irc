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

		void	run(void);

		std::map<int, Client *>	getClients(void) const;

	private:

		int			_port;
		std::string	_password;
		Socket		_socket;
		fd_set		_readFds;
		Irc			_command;
		std::map<int, Client *>	_clients;
		std::vector<Channel>	_channels;

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

// Cette fontion va servir pour le RPL_CREATED(nick, datetime), c'est le 003
// #include <time>

// std::string const	getTime() {

// 	time_t	t(time(NULL));
// 	std::string	res(ctime(&t));
// 	res.erase(res.end() - 1);
	
// 	return (res);
// }