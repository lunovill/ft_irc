#include "Irc.hpp"
#include "Server.hpp"

const	Irc::commands Irc::cmdList[] = {
	{"NICK", &Irc::NICK},
	{"PASS", &Irc::PASS},
	{"USER", &Irc::USER},
	// {"JOIN", &JOIN},
	// {"LIST", &LIST},
	// {"EXIT", &EXIT},
	// {"PING", &PING_PONG},
	// {"PONG", &PING_PONG},
	// {"PRIVMSG", &PRIVMSG},
	// {"PART", &PART},
	// {"TOPIC", &TOPIC},
	// {"MODE", &MODE},
	// {"KICK", &KICK}
	{"", NULL}
};

/********************************************************************************/
/* ------------------------------- CONSTRUCTOR -------------------------------- */
/********************************************************************************/

Irc::Irc(Server *server) : _server(server) { return; }

/********************************************************************************/
/* -------------------------------- DESTRUCTOR -------------------------------- */
/********************************************************************************/

Irc::~Irc(void) { return; }

/********************************************************************************/
/* --------------------------------- METHODS ---------------------------------- */
/********************************************************************************/


Irc::CommandFt	Irc::find(std::string const &input) const {
	for (unsigned int i = 0; !Irc::cmdList[i].name.empty(); ++i) {
		    std::string cmdName(Irc::cmdList[i].name);
        if (input.compare(0, cmdName.length(), cmdName) == 0 && (input[cmdName.length()] == ' ' || input[cmdName.length()] == '\r' || input[cmdName.length()] == '\n')) {
			std::cout << "Commande server : "<< cmdName << " ---- Command Client : " << input << CLRF;
            return Irc::cmdList[i].cmd;
        }
    }
	return NULL;
};

// A faire en priorite

void	Irc::NICK(int fd, Client &client) {
	std::string	nickname = client.input.substr(5, client.input.length() - 5);
	if (nickname.empty()) {
		client.output += ERR_NONICKNAMEGIVEN(client.nickname);
		// send(fd, output.c_str(), output.length(), 0);
		return;
	}
	for (unsigned int i = 0; i < nickname.length(); ++i)
		if (!std::isalnum(nickname[i]) && nickname[i] != '-' && nickname[i] != '_') {
			client.output += ERR_ERRONEUSNICKNAME(client.nickname, nickname);
			// send(fd, output.c_str(), output.length(), 0);
			return;
		}
	std::map<int, Client *> clients = _server->getClients();
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->nickname == nickname) {
			client.output += ERR_NICKNAMEINUSE(client.nickname, nickname);
			// send(fd, output.c_str(), output.length(), 0);
			return;
		}}
	client.nickname = nickname;
	std::cout << "Client " << fd << " changed nickname to " << nickname << std::endl;
	return;
}

void	Irc::PASS(int fd, Client &client) { return; }
void	Irc::USER(int fd, Client &client) { return; }

/********************************************************************************/
