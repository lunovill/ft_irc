#pragma once

# include <vector>
# include "Client.hpp"

class   Channel {

	private:


	public:

		const std::string	name;
		std::string			password;
		Channel(std::string name);
		~Channel(void);

		std::vector<Client> clients;
};



