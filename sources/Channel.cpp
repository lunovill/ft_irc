#include "Channel.hpp" 
#include "Utils.hpp" 

/********************************************************************************/
/* ------------------------------- CONSTRUCTOR -------------------------------- */
/********************************************************************************/

Channel::Channel(std::string name) : _name(name), _clientLimit(0) { return; }

Channel::Channel(std::string name, std::string password) : _name(name), _password(password), _mode("k"), _clientLimit(0) { return; }

/********************************************************************************/
/* -------------------------------- DESTRUCTOR -------------------------------- */
/********************************************************************************/


Channel::~Channel(void) { return; }

/********************************************************************************/
/* --------------------------------- METHODS ---------------------------------- */
/********************************************************************************/

bool		Channel::addClient(int fd, Client &client) {
	if (_mode.find('l') == std::string::npos || (_mode.find('l') != std::string::npos && _clients.size() < _clientLimit)) {
		_clients[fd] = &client;
		return true;
	}
	return false;
}

bool	Channel::findClient(int fd) const { return (_clients.find(fd) == _clients.end()) ? false : true; }

bool	Channel::eraseClient(int fd) { 
	_clients.erase(_clients.find(fd));
	return _clients.size() ? true : false;
}

bool	Channel::findClient(std::string const &nickname) const {
	for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (nickname == it->second->nickname)
			return true;
	return false;
}

void        Channel::eraseMode(char const &mode) {
	if (!_mode.empty() && (_mode.find(mode) != std::string::npos))
		 _mode.erase(_mode.find(mode), 1); 
}

std::string	Channel::clientList(std::string const &firstName) const {
	std::string clientsNames = firstName;
	for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (firstName != it->second->nickname && it->second->mode.find('i') == std::string::npos)
			clientsNames += " " + it->second->nickname;
	return clientsNames;
}

void	Channel::sendAll(int const &senderFd, Client const &sender, std::string const &message, bool const &oper) const { 
	for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (senderFd != it->first && ((oper && it->second->mode.find('o') != std::string::npos) || (!oper))) {
			std::string output = std::string(":") + sender.nickname + std::string("!~u@") + sender.hostname + std::string(".irc ") + message + CLRF;
			send(it->first, output.c_str(), output.length(), 0);
		}
	return;
}

/********************************************************************************/
/* --------------------------------- ACCESSOR --------------------------------- */
/********************************************************************************/

std::string Channel::getName(void) const { return _name; }

std::string Channel::getPass(void) const { return _password; }

std::string Channel::getMode(void) const { return _mode; }

void        Channel::setMode(char const &mode) { _mode += mode; }

void        Channel::setPass(std::string &password) { _password = password; }

void        Channel::setClientLimit(std::string &clientLimit) { _clientLimit = to_sizeT(clientLimit); }

/********************************************************************************/