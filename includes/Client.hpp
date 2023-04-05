#pragma once

# include <iostream>

class	Client {

	public:

		Client(void);
		~Client(void);

		bool		cmdRegister[4];
		std::string	nickname;
		std::string	username;
		std::string	hostname;
		std::string	realname;
		std::string	servername;
		std::string input;
		std::string output;

		bool	isRegister(void);
};
