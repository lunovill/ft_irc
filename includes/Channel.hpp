#pragma once

# include <map>
# include "Client.hpp"

class	Channel {

	public:

		Channel(std::string name);
		Channel(std::string name, std::string password);
		~Channel(void);

		std::map<int, Client *> clients;

	private:

		const std::string	_name;
		const std::string	_password;
};
