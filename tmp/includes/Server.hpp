#pragma once
# include <cerrno>
# include <cstdlib>
# include <sys/types.h>
# include <sys/socket.h>
# include <cstring>
# include <fcntl.h>
# include <netinet/in.h>
# include <iostream>
# include <sstream>
# include <string>
# include <algorithm>
# include <sstream>
# include <unistd.h>
# include <vector>
# include <Channel.hpp>

# define MAX_CLIENT 1024
# define BUFFER_SIZE 512

enum CMD {
    NICK,
    USER,
    CAP_LS,
	PRIVMSG,
	JOIN,
	NB_OF_CMDS
};

class Server
{
	private :

		int									_port;
		int									_sockfd;
		int									_client_count;
		sockaddr_in							_server_addr;
		fd_set								_readFds;
		std::map<int, Client>::iterator		_current_client;
		std::map<int, Client>				_clients;
		std::string							_password;
		std::string							_servername;
		std::string							_cmd_str[NB_OF_CMDS];
		std::vector<Channel>				_channels;
	
		/*
		std::map<Channel, std::map<int, Client>> _clientsPerChannel;
		
		bool modifyClientInfo(int fd, int infoToChange, std::string newInfo) {
			//check si le fd peut ne pas etre dans la map
			switch

			map[fd].
		}
		*/
		
		bool	(Server::*_cmd_list[NB_OF_CMDS])(std::string &);
		bool	_join(std::string &cmd);
		bool	_initSock(void);
		bool	_initBind(void);
		bool	_initListen(void) const;
		bool	_acceptClient(int &clientFd) const;
		bool	_nick(std::string &cdm);
		bool	_user(std::string &cdm);
		bool	_privmsg(std::string &cdm);
		bool	_capls(std::string &cdm);
		int		_commandFind(std::string &cmd) const;
		void	_commandServer(std::vector<std::string>  &tabstr);
		void	_dataRecv(void);
		
	public :

		Server(int port, std::string password);
		~Server(void);

		bool	event(void);
};

template<typename T>
std::vector<std::string> ft_split(T msg, char c = ' ')
{

	std::string line;
    std::vector<std::string> vec;
    std::istringstream stream(msg);

    while (std::getline(stream, line, c))
        if (!line.empty())
            vec.push_back(line);
    return vec;
}

template<typename T>
std::string	to_string(T arg)
{
	std::stringstream ss;
	ss << arg;
	std::string str = ss.str();
	return str;
}