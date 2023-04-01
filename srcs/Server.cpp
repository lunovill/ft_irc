#include "Server.hpp"

/********************************************************************************/
/* ------------------------------- CONSTRUCTOR -------------------------------- */
/********************************************************************************/

Server::Server(int port, std::string password) : _port(port), _client_count(0),  _password(password) {
	_initSock();
	_initBind();
	_initListen();

	Server::_cmd_list[NICK] = &Server::_nick;
	Server::_cmd_list[USER] = &Server::_user;
	Server::_cmd_list[CAP_LS] = &Server::_capls;
	Server::_cmd_list[PRIVMSG] = &Server::_privmsg;

	_cmd_str[NICK] = "NICK";
	_cmd_str[USER] = "USER";
	_cmd_str[CAP_LS] = "CAP LS";
	_cmd_str[PRIVMSG] = "PRIVMSG";
	return;
}

/********************************************************************************/
/* -------------------------------- DESTRUCTOR -------------------------------- */
/********************************************************************************/

Server::~Server(void) { return; }

/********************************************************************************/
/* --------------------------------- METHODS ---------------------------------- */
/********************************************************************************/

bool	Server::_initSock(void) {
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == -1) {
		std::cerr << "\033[1;31m" << "Error: Failed to creat socket: " << strerror(errno) << std::endl;
		return false;
	}

	int flags = fcntl(_sockfd, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "\033[1;31m" << "Error: Failed to get socket flags: " << strerror(errno) << std::endl;
		return false;
	}

	if (fcntl(_sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "\033[1;31m" << "Error: Failed to set socket noblocking: " << strerror(errno) << std::endl;
		return false;
	}

	int reuseaddr = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) { 
			std::cerr << "Error setting socket options: " << strerror(errno) << std::endl;
			return false;
	}

	std::cout << "Socket server created" << std::endl;
	return true;
}

bool Server::_initBind(void) {
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	_server_addr.sin_port = htons(_port);
	if (bind(_sockfd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) == -1) {
		std::cerr << "Error: Failed to bind socket: " << strerror(errno) << std::endl;
		return false;
	}
	std::cout << "Socket server bind" << std::endl;
	return true;
}

bool Server::_initListen(void) const  {
	if (listen(_sockfd, MAX_CLIENT) == -1) {
		std::cerr << "Error: Failed to listen on socket: " << strerror(errno) << std::endl;
		return false;
	}
	std::cout << "Socket server listen" << std::endl;
	return true;
}

bool Server::_acceptClient(int &client_fd) const {
	client_fd = accept(_sockfd, NULL, NULL);
	if (client_fd == -1) {
		std::cerr << "Error: Failed to accept connection :" << strerror(errno) << std::endl;
		return false;
	}
	std::cout << "Socket server accept connection" << std::endl;
	return true;
}

bool Server::_nick(std::string &cmd) {
	std::vector<std::string> res = ft_split(cmd);

	if (res.size() != 2)
		return false;
	if (res.size() == 1) {
		send(_current_client->first, "431", 3, 0);
		return false;
	}
	for (std::string::iterator it = res[1].begin(); it != res[1].end(); ++it)
		if (!isdigit(*it) && !isalpha(*it)) {
			send(_current_client->first, "432", 3, 0);
			return false;
		}
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (it->second.nickname == res[1]) {
			send(_current_client->first, "433", 3, 0);
			return false;
		}
	std::string nickmsg = _current_client->second.nickname + " NICK " + res[1];

	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		send(it->first, nickmsg.c_str(), nickmsg.length(), 0);
	_current_client->second.nickname = res[1];
	return true;
}

bool Server::_user(std::string &cmd)
{
	std::vector<std::string> res = ft_split(cmd);
	

	return true;
}

bool Server::_privmsg(std::string &cmd)
{
	(void)cmd;
	return true;
	// ...
}

bool Server::_capls(std::string &cmd)
{
	(void)cmd;
	return true;
	// ...
}


bool	Server::_join(std::string &cmd) {
	std::vector<std::string> cmd_split = ft_split(cmd);

	std::vector<std::string>	names = ft_split(cmd_split[1], ',');
	std::vector<std::string>	password = ft_split(cmd_split[2], ',');
	for (unsigned int i = 0; i < names.size(); i++) {
		std::vector<Channel>::iterator it;
		for (it = _channels.begin(); it != _channels.end(); ++it) {
			if (it->name == names[i]) {
				break;
        	}
			if (it == _channels.end()) {
        		Channel newChannel(names[i]);
				_channels.push_back(newChannel);
				it = _channels.end() - 1;
    		}
			if (!it->password.empty() && it->password != password[i]) {
				//erreur
				return false;
			}
			if (std::find(it->clients.begin(), it->clients.end(), _current_client->second) != it->clients.end()) {
				//erreur
				return false;
			}

			it->clients.push_back(_current_client->second);
			std::string message = ":" + _current_client->second.nickname + "!" + _current_client->second.username + "@" + _current_client->second.hostname + " JOIN " + names[i] + "\r\n";
			for (std::vector<Client>::iterator clientIt = it->clients.begin(); clientIt != it->clients.end(); ++clientIt) {
				if (*clientIt != _current_client->second)
					send(clientIt->fd, message.c_str(), message.length(), 0);
			}
		}
	}
	return true;
}

int Server::_commandFind(std::string &cmd) const {
	std::string res;

	for (unsigned int i = 0; i < NB_OF_CMDS; i++) {
		res = cmd.substr(0, _cmd_str[i].length());
		if (res == _cmd_str[i] && (!cmd[res.length()] || (cmd[res.length()] == '\n' || cmd[res.length()] == ' ' || cmd[res.length()] == '\r'))) {
			if (_current_client->second.is_register() == -1 && res == "CAP LS")
				_current_client->second.cmd_register[0] = true;
			else if (_current_client->second.is_register() == -1 && res == "NICK")
				_current_client->second.cmd_register[1] = true;
			else if (_current_client->second.is_register() == -1 && res == "USER")
				_current_client->second.cmd_register[2] = true;
	
			std::cout << "Client : " << _current_client->first << " Commande server : "<< _cmd_str[i] << " ---- Command Client : " << cmd << std::endl;
			return i;
		}
	}
	std::string error = "Error: command not found: " + cmd;
	send(_current_client->first, error.c_str(), error.length(), 0);
	return -1;
}

void Server::_commandServer(std::vector<std::string>  &tabstr) {
	for (unsigned int i = 0; i < tabstr.size(); i++) {
		int command = _commandFind(tabstr[i]);
		if (command != -1)
			((this->*_cmd_list[command])(tabstr[i]));
		if (_current_client->second.is_register() == 1) {
			std::string res =  _current_client->second.realname + " :Welcome to the IRC Network, " + _current_client->second.nickname + "!" + _current_client->second.username + "@" + _current_client->second.hostname + "\n";
			send(_current_client->first, "001\n", 4, 0);
			send(_current_client->first, res.c_str(), res.length(), 0);
			_current_client->second.cmd_register[3] = true;
		}
	}
}



void	Server::_dataRecv(void) {
	char	buff[BUFFER_SIZE];
	if (!_clients.empty())
	{
		for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
			_current_client = it;
			if (FD_ISSET(it->first, &_readFds)) {
				int bytes_receved = recv(it->first, buff, BUFFER_SIZE, 0);
				if (bytes_receved <= 0) {
					std::cout << "Client " << it->first << " desconnected" << std::endl;
					close(it->first);
					_clients.erase(it->first);
					_client_count--;
				} else {
					buff[bytes_receved] = '\0';
					std::vector<std::string> split = ft_split(buff, '\n');
					_commandServer(split);
				}
			}
		}
	}
	return;
}

bool	Server::event(void) {
	int		client_fd;
	int		max_fd;

	while(true) {
		FD_ZERO(&_readFds);
		FD_SET(_sockfd, &_readFds);
		max_fd = _sockfd;

		for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
			FD_SET(it->first, &_readFds);
		if (_client_count)
			max_fd = _clients.rbegin()->first;
		int activity = select(max_fd + 1, &_readFds, NULL, NULL, NULL);
		if (activity < 0) {
			std::cerr << "Error: Failed to select: " << strerror(errno) << std::endl;
			return false;
		}
		if (FD_ISSET(_sockfd, &_readFds)) {
			if (_acceptClient(client_fd)) {
				Client client(client_fd);
				_clients.insert(std::make_pair(client_fd, Client(client_fd)));
				_client_count++;
				std::cout << "Client " << client_fd << " connected" << std::endl;
			}
		}
		_dataRecv();
	}
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->first);
	_client_count = 0;
	close(_sockfd);
	std::cout << "Server closed" << std::endl;
	return true;
}

/********************************************************************************/
