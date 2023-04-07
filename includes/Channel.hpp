#pragma once

# include <map>
# include "Client.hpp"

class	Channel {

	public:

		Channel(std::string name);
		Channel(std::string name, std::string password);
		~Channel(void);

		std::string	topic;

		bool		addClient(int fd, Client &client);

		std::string getName(void) const;
		std::string getPass(void) const;
		std::string getMode(void) const;
		void		setMode(std::string mode);

		std::map<int, Client *> clients;

	private:

		const std::string	_name;
		const std::string	_password;
		std::string			_mode;
		unsigned int		_clientLimit;
};
