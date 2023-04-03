#include "Irc.hpp"

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

Irc::Irc(void) { return; }

/********************************************************************************/
/* -------------------------------- DESTRUCTOR -------------------------------- */
/********************************************************************************/

Irc::~Irc(void) { return; }

/********************************************************************************/
/* --------------------------------- METHODS ---------------------------------- */
/********************************************************************************/


Irc::CommandFt	Irc::find(std::string const &input) const {
		std::string res;

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

void	Irc::NICK(int fd, Client &client) { return; }

// Voici l'ancienne commande nick, a tradiure
// Utilise CLRF au lien de "\r\n" (c'est une convention)
// Ce que le client envoie se trouve dans sa classe, c'est la varible input

// bool Server::_nick(std::string &cmd) {
// 	std::vector<std::string> res = ft_split(cmd);
// 	res[1] = res[1].substr(0, res[1].length() - 1);

// 	if (res.size() != 2)
// 		return false;
// 	if (res.size() == 1) {
// 		send(_current_client->first, "431", 3, 0);
// 		return false;
// 	}
// 	for (std::string::iterator it = res[1].begin(); it != res[1].end(); ++it)
// 		if (!isdigit(*it) && !isalpha(*it)) {
// 			send(_current_client->first, "432", 3, 0);
// 			return false;
// 		}
// 	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
// 	{
// 		if (it->second.nickname == res[1])
// 		{
// 			res[1] = res[1].substr(0, res[1].length() - 2) + to_string(_current_client->first);
// 			send(_current_client->first, "433", 3, 0);
// 			return false;
// 		}
// 	}
// 	std::string nickmsg = _current_client->second.nickname + " NICK " + res[1] + "\r\n";

// 	// for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
// 	send(_current_client->first, nickmsg.c_str(), nickmsg.length(), 0);
// 	_current_client->second.nickname = res[1];
// 	// std::cout << "index " << _current_client->first << " value " << _current_client->second.nickname << std::endl; 
// 	return true;
// }


void	Irc::PASS(int fd, Client &client) { return; }
void	Irc::USER(int fd, Client &client) { return; }

/********************************************************************************/
