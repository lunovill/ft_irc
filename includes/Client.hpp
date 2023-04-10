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
		std::string output;
		std::string mode;
		// Tableau qui permet de savoir si le client a deja utilise NICK USER PASS (correct) et est deja register
		bool		cmdRegister[4];

		// Verifie si le client peut etre enregistre si il ne l'ai pas deja
		bool	isRegister(void);
};
