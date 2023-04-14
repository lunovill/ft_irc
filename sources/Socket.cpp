#include "Socket.hpp"

/********************************************************************************/
/* ------------------------------- CONSTRUCTOR -------------------------------- */
/********************************************************************************/

Socket::Socket(int port) : _port(port) {
	_initSocket();
	_initBind();
	_initListen();
	return;
}

/********************************************************************************/
/* -------------------------------- DESTRUCTOR -------------------------------- */
/********************************************************************************/

Socket::~Socket(void) { return; }

/********************************************************************************/
/* --------------------------------- METHODS ---------------------------------- */
/********************************************************************************/

void	Socket::_initSocket(void) {
	_fd = socket(AF_INET, SOCK_STREAM, 0);

	std::string	error;
	if (_fd == -1) {
		error = std::string("\033[0;2;3msocket(): Failed to creat socket: ") + strerror(errno);
		throw std::runtime_error(error);
	}

	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
		error = std::string("\033[0;2;3mfcnl(): Failed to set socket noblocking: ") + strerror(errno);
		throw std::runtime_error(error);
	}

	int reuseaddr = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) { 
		error = std::string("\033[0;2;3msetsockopt(): Failed to set socket options: ") + strerror(errno);
		throw std::runtime_error(error);
	}

	std::cout << "Socket server created" << std::endl;
	return;
}

void	Socket::_initBind(void) {
	memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr.s_addr = INADDR_ANY;
	_sockAddr.sin_port = htons(_port);

	std::string	error;
	if (bind(_fd, (struct sockaddr *)&_sockAddr, sizeof(_sockAddr)) == -1) {
		error = std::string("\033[0;2;3mbind(): Failed to bind socket: ") + strerror(errno);
		throw std::runtime_error(error);
	}
	std::cout << "Socket server bind" << std::endl;
	return;
}

void	Socket::_initListen(void) const  {
	std::string	error;
	if (listen(_fd, MAX_CLIENT) == -1) {
		error = std::string("\033[0;2;3mlisten(): Failed to listen on socket: ") + strerror(errno);
		throw std::runtime_error(error);
	}
	std::cout << "Socket server listen" << std::endl;
	return;
}

/********************************************************************************/
/* --------------------------------- ACCESSOR --------------------------------- */
/********************************************************************************/

int	Socket::getFd(void) const { return _fd; }

/********************************************************************************/
