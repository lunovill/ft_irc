#pragma once
# include <iostream>

class	Client {

	public:
		Client(void);
		~Client(void);
             
		std::string	nickname;
		std::string	username;
		std::string	hostname;
		std::string	realname;
		std::string	servername;
		std::string input;

	private:

		bool	_isRegister;
};
