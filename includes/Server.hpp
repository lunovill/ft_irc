#pragma once
# include <cerrno>
# include <cstdlib>
# include <sys/types.h>
# include <sys/socket.h>
# include <cstring>
# include <fcntl.h>
# include <netinet/in.h>
# include <map>
# include <vector>
# include <string>
# include <sstream>
# include <unistd.h>
# include <Client.hpp>

# define MAX_CLIENT 1024
# define BUFFER_SIZE 512

enum Color {
    NICK,
    USER,
    CAP_LS,
	PRIVMSG,
	NB_OF_CMDS
};


class Server
{
	private :

		int														_port;
		int														_sockfd;
		int														_client_count;
		std::map<int, Client>::iterator							_current_client;
		sockaddr_in												_server_addr;
		fd_set													_readFds;
		std::string												_password;
		std::string												_servername;
		std::string												_cmd_str[NB_OF_CMDS];
		bool													(Server::*_cmd_list[NB_OF_CMDS])(std::string &);
		bool													_initSock(void);
		bool													_initBind(void);
		bool													_initListen(void) const;
		bool													_acceptClient(int &clientFd) const;
		void													_dataRecv(void);
		void													_commandServer(std::vector<std::string>  &tabstr);
		int														_commandFind(std::string &cmd) const;
		bool													_nick(std::string &cdm);
		bool													_user(std::string &cdm);
		bool													_privmsg(std::string &cdm);
		bool													_capls(std::string &cdm);

	public :

		Server(int port, std::string password);
		~Server(void);

		std::map<int, Client> clients;

		bool	event(void);
};

template<typename T>
std::vector<std::string> ft_split(T msg, char c = ' ') {

	std::string line;
    std::vector<std::string> vec;
    std::istringstream stream(msg);

    while (std::getline(stream, line, c))
        if (!line.empty())
            vec.push_back(line);
    return vec;
}