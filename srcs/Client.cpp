#include "Client.hpp"

Client::Client(void) : nickname("default"), username("default"), realname("default")
{
	cmd_register[0] = false;
	cmd_register[1] = false;
	cmd_register[2] = false;
	cmd_register[3] = false;
    return;
}

Client::~Client(void) 
{ 
	return;
}

int        Client::is_register(void)
{
    if (cmd_register[0] && cmd_register[1] && cmd_register[2]) {
		if (cmd_register[3])
        	return 0;
		else
			return 1;
	}
    return -1;
}
