#include "Server.hpp"
#include "Irc.hpp"

/********************************************************************************/
/* ------------------------------- CONSTRUCTOR -------------------------------- */
/********************************************************************************/

Server::Server(int port, std::string password) : _port(port), _password(password), _socket(port), _command(this) { return; }

/********************************************************************************/
/* -------------------------------- DESTRUCTOR -------------------------------- */
/********************************************************************************/

Server::~Server(void) { return; }

/********************************************************************************/
/* --------------------------------- METHODS ---------------------------------- */
/********************************************************************************/

void	Server::_acceptClient(int &clientFd) const {
	clientFd = accept(_socket.getFd(), NULL, NULL);

	std::string	error;
	if (clientFd == -1) {
		error = std::string("\033[0;2;3maccept(): Failed to accept connection :") + strerror(errno);
		throw std::runtime_error(error);
	}
	std::cout << "Socket server accept connection" << std::endl;
	return ;
}

void Server::_commandRun(std::map<int, Client *>::iterator &client, std::vector<std::string>  &inputs)
{	
	for (unsigned int i = 0; i < inputs.size(); i++) {
		client->second->input = inputs[i].substr(0, inputs[i].length() - 1);
		Irc::CommandFt cmd = _command.find(client->second->input);
		if (cmd) (_command.*cmd)(client->first, *client->second);
		else client->second->output += ERR_UNKNOWNCOMMAND(client->second->nickname, to_split(client->second->input)[0]);
	}
	if (client->second->isRegister()) {
		client->second->output += RPL_WELCOME(client->second->nickname, client->second->username, client->second->hostname);
		client->second->output += RPL_YOURHOST(client->second->nickname);
		client->second->output += RPL_CREATED(client->second->nickname, getTime());
		client->second->output += RPL_MYINFO(client->second->nickname);
		client->second->cmdRegister[3] = true;

	}
	send(client->first, client->second->output.c_str(), client->second->output.length(), 0);
	std::cout << client->second->output << std::endl;
	client->second->output.erase();
}

void Server::_dataRecv(void) {
	char buff[BUFFER_SIZE];
	std::map<int, Client *>::iterator it = _clients.begin();
	while (it != _clients.end()) {
		if (FD_ISSET(it->first, &_readFds)) {
			int bytes_received = recv(it->first, buff, BUFFER_SIZE, 0);
			if (bytes_received <= 0) {
				std::cout << "Client " << it->first << " desconnected" << std::endl;
				close(it->first);
				delete it->second;
				_clients.erase(it++);
			} else {
				buff[bytes_received] = '\0';
				it->second->input = std::string(buff);
				std::vector<std::string> inputs = to_split(buff, '\n');
				_commandRun(it, inputs);
				// Ce morceau de code pemet de faire communiquer tout les utilisateurs entre eux
				// std::cout << "Client " << it->first << " sent: " << buff;
				// for (std::map<int, Client *>::iterator it2 = _clients.begin(); it2 != _clients.end(); ++it2)
				// 	if (it2->first != it->first)
				// 		send(it2->first, buff, bytes_received, 0);
				++it;
			}
		} else {
			++it;
		}
	}
	return;
}

bool	Server::findClientNick(std::string const nickname) const {
	for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (it->second->nickname == nickname)
			return true;
	return false;
}

void	Server::addChannel(Channel *channel) { _channels.push_back(channel); }

void	Server::sendAll(int fd, Client &client, std::string const message) const { 
	for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (fd !=  it->first) {
			std::string	output = RPL_PREFIX("", it->second->nickname) + " :" + client.nickname + message + CLRF;
			send(it->first, output.c_str(), output.size(), 0);
		}
	return;
}

void	Server::eraseChannel(Channel *channel) {
	delete channel;
	_channels.erase(std::find(_channels.begin(), _channels.end(), channel));
	return;
}

void	Server::run(void) {
	int		client_fd;
	int		max_fd;

	while(true) {
		FD_ZERO(&_readFds);
		FD_SET(_socket.getFd(), &_readFds);
		max_fd = _socket.getFd();

		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
			FD_SET(it->first, &_readFds);
			max_fd = std::max(max_fd, it->first);
		}

		int activity = select(max_fd + 1, &_readFds, NULL, NULL, NULL);
		if (activity < 0) {
			std::cerr << "\033[0;1;2;4mError:\033[0m \033[0;2;3mselect(): Failed to select: " << strerror(errno) << std::endl;
			break;
		}

		if (FD_ISSET(_socket.getFd(), &_readFds)) {
			try { _acceptClient(client_fd); } catch (std::runtime_error &e) {
				std::cerr << "\033[0;1;2;4mError:\033[0m " << e.what() << std::endl;
				break;
			}
			_clients[client_fd] = new Client;
			std::cout << "Client " << client_fd << " connected" << std::endl;
		}
		_dataRecv();
	}
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		close(it->first);
		delete it->second;
	}
	for (std::vector<Channel *>::iterator it = _channels.end(); it != _channels.end(); ++it)
		delete *it;
	_clients.clear();
	close(_socket.getFd());
	std::cout << "Server closed" << std::endl;
	return;
}

/********************************************************************************/
/* --------------------------------- ACCESSOR --------------------------------- */
/********************************************************************************/

const std::string Server::getPass() const { return _password; }

Channel	*Server::getChannel(std::string const &name) const {
	for (std::vector<Channel *>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
		if (name == (*it)->getName())
			return *it;
	return NULL;
}

/********************************************************************************/

#include <ctime>

std::string const	getTime() {

	time_t	t(time(NULL));
	std::string	res(ctime(&t));
	res.erase(res.end() - 1);
	
	return (res);
}
