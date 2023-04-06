#include "Channel.hpp" 

/********************************************************************************/
/* ------------------------------- CONSTRUCTOR -------------------------------- */
/********************************************************************************/

Channel::Channel(std::string name) : _name(name), _clientLimit(-1) { return; }

Channel::Channel(std::string name, std::string password) : _name(name), _password(password), _clientLimit(-1) { return; }

/********************************************************************************/
/* -------------------------------- DESTRUCTOR -------------------------------- */
/********************************************************************************/


Channel::~Channel(void) { return; }

/********************************************************************************/
/* --------------------------------- METHODS ---------------------------------- */
/********************************************************************************/

bool		Channel::addClient(int fd, Client &client) {
	if (_clientLimit == -1 || (_clientLimit != -1 && clients.size() < _clientLimit)) {
		clients[fd] = &client;
		return true;
	}
	return false;
}

/********************************************************************************/
/* --------------------------------- ACCESSOR --------------------------------- */
/********************************************************************************/

std::string Channel::getName(void) const { return _name; }

std::string Channel::getPass(void) const { return _password; }

std::string Channel::getMode(void) const { return _mode; }

void        Channel::setMode(std::string mode) { _mode = mode; }

/********************************************************************************/