#pragma once

# include <cerrno>
# include <cstring>
# include <fcntl.h>
# include <iostream>
# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/types.h>

# define MAX_CLIENT 1024

class	Socket {

	public:

		Socket(int port);
		~Socket(void);

		int	getFd(void) const;

	private:

		int			_port;
		int			_fd;
		sockaddr_in	_sockAddr;

		void	_initSocket(void);
		void	_initBind(void);
		void	_initListen(void) const;
};
