#include "Channel.hpp" 

/********************************************************************************/
/* ------------------------------- CONSTRUCTOR -------------------------------- */
/********************************************************************************/

Channel::Channel(std::string name) : _name(name), _clientLimit(0) { return; }

Channel::Channel(std::string name, std::string password) : _name(name), _password(password), _clientLimit(0), _mode("k") { return; }

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

std::string	Channel::clientList(std::string const &firstName) const {
	std::string clientsNames = firstName;
	for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (firstName != it->second->nickname)
			clientsNames += " " + it->second->nickname;
	return clientsNames;
}

void        Channel::unsetMode(char mode) { _mode.erase(mode); }

/********************************************************************************/
/* --------------------------------- ACCESSOR --------------------------------- */
/********************************************************************************/

std::string Channel::getName(void) const { return _name; }

std::string Channel::getPass(void) const { return _password; }

std::string Channel::getMode(void) const { return _mode; }

void        Channel::setMode(std::string const mode) { _mode = mode; }

/********************************************************************************/