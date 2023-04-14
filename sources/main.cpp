#include <cstdlib>

#include "Server.hpp"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " port password" << std::endl;
		return 1;
	}

	for (char *c = argv[1]; *c != '\0'; ++c) {
		if (!isdigit(*c)) {
			std::cerr << "\033[0;1;2;4mError:\033[0m \033[0;2;3mInvalid port " << argv[1] << std::endl;
			return 1;
		}
	}
	int port = std::atoi(argv[1]);

	Server irc(port, argv[2]);
	irc.run();
	return 0;
}