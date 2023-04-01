#include "Client.hpp"

Client::Client(int fd) : fd(fd) {
	cmd_register[0] = false;
	cmd_register[1] = false;
	cmd_register[2] = false;
	cmd_register[3] = false;
    return;
}

Client::~Client(void) { return; }

int        Client::is_register(void) {
    if (cmd_register[0] && cmd_register[1] && cmd_register[2]) {
		if (cmd_register[3])
        	return 0;
		else
			return 1;
	}
    return -1;
}

bool	Client::operator!=(const Client& other) const {
	return fd != other.fd;
}

bool Client::operator==(const Client& other) const {
    return fd == other.fd;
}
