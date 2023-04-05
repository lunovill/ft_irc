#include "Irc.hpp"
#include "Server.hpp"

const	Irc::commands Irc::cmdList[] = {
	{"PASS", &Irc::PASS},
	{"NICK", &Irc::NICK},
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

void Irc::PASS(int fd, Client &client)
{
	if (client.cmdRegister[3] == true) {
		client.output += ERR_ALREADYREGISTRED(client.nickname);
		return;
	}
	std::string password = client.input.substr(5, client.input.length() - 5);
	if (password.empty()) {
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "PASS");
		return;
	}
	if (password != _server->getPass()) {
		client.output += ERR_PASSWDMISMATCH(client.nickname);
		return;
	}
	if (client.cmdRegister[0] != true)
		client.cmdRegister[0] = true;
	std::cout << "Password is correct" << std::endl;
	return;
}

void	Irc::NICK(int fd, Client &client) {
	std::string	nickname = client.input.substr(5, client.input.length() - 5);
	if (nickname.empty()) {
		client.output += ERR_NONICKNAMEGIVEN(client.nickname);
		return;
	}
	for (unsigned int i = 0; i < nickname.length(); ++i)
		if (!std::isalnum(nickname[i]) && nickname[i] != '-' && nickname[i] != '_') {
			client.output += ERR_ERRONEUSNICKNAME(client.nickname, nickname);
			return;
		}
	std::map<int, Client *> clients = _server->getClients();
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
		if (it->second->nickname == nickname) {
			client.output += ERR_NICKNAMEINUSE(client.nickname, nickname);
			std::stringstream index;
			index << (fd - 4);
			nickname += index.str();
			break;
		}
	client.nickname = nickname;
	if (client.cmdRegister[1] != true)
		client.cmdRegister[1] = true;	
	std::cout << "Client " << fd << " changed nickname to " << nickname << std::endl;
	return;
}

void Irc::USER(int fd, Client &client)
{
	if (client.cmdRegister[3] == true) {
		client.output += ERR_ALREADYREGISTRED(client.nickname);
		return;
	}
	std::vector<std::string> parametre = to_split(client.input);
	if (parametre.size() != 6)
	{
		client.output += ERR_NEEDMOREPARAMS(client.nickname, parametre[0]);
		send(fd, client.output.c_str(), client.output.length(), 0);
		return;
	}

	for (int i = 0; i < parametre.size(); i++)
	{
		for (unsigned int j = 0; j < parametre[i].length(); j++)
		{
			// if ((i == 1 || i == 2) && !std::isalnum(parametre[i][j]) && parametre[i][j] != '-' && parametre[i][j] != '_')
			// {
			// 	if (i == 1)
			// 		client.output += ERR_ERRONEUSNICKNAME(client.nickname, parametre[i]);
			// 	else
			// 		client.output += ERR_ERRONEUSUSERNAME(client.username, parametre[i]);
			// 	// send(fd, client.output.c_str(), client.output.length(), 0);
			// 	return;
			// }
			// if (i == 3 && (!isIpAddress(parametre[3]) && parametre[3] != "localhost"))
			// 	return;
			if ((i == 4 || i == 5))
			{
				if (i == 4 && parametre[i][0] != ':')
				{
					client.output += ERR_NEEDMOREPARAMS(client.nickname, parametre[0]);
					// send(fd, client.output.c_str(), client.output.length(), 0);
					return;
				}
				else
					j++;
				// if (!std::isalpha(parametre[i][j]))
				// {
				// 	client.output += ERR_ERRONEUSREALNAME(client.username, parametre[i]);
				// 	// send(fd, client.output.c_str(), client.output.length(), 0);
				// 	return;
				// }
			}
		}
	}

	// std::cout << "ICI 1 " << std::endl;
	// std::map<int, Client *> clients = _server->getClients();
	// for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	// {
	// 	if (it->second->nickname == parametre[1])
	// 	{
	// 		client.output += ERR_ALREADYREGISTRED(client.nickname);
	// 		// send(fd, client.output.c_str(), client.output.length(), 0);
	// 		return;
	// 	}
	// 	if (it->second->username == parametre[2])
	// 	{
	// 		client.output += ERR_ALREADYREGISTRED(client.nickname);
	// 		// send(fd, client.output.c_str(), client.output.length(), 0);
	// 		return;
	// 	}
	// }

	if (client.nickname.empty())
		client.nickname = parametre[1];
	client.username = parametre[2];
	client.hostname = parametre[3];
	client.realname = parametre[4] + " " + parametre[5];
	if (client.cmdRegister[2] != true)
		client.cmdRegister[2] = true;
	std::cout << "nickname " << client.nickname << " username " << client.username << " hostname " << client.hostname << " realname " << client.realname << std::endl;
}


// En cours...
// void	Server::JOIN(int fd, Client &client) {
// 	std::vector<std::string>	input = to_split(client.input);
// 	std::vector<std::string>	names = ft_split(input[1], ',');
// 	std::vector<std::string>	passwords = ft_split(input[2], ',');
// 	std::vector<Channel *>		channel = _server.getChannel();
// 	std::vector<Channel *>::iterator it;
// 	for (unsigned int i = 0; i < names.size(); i++) {
// 		for (it = channels.begin(); it != channels.end(); ++it) {
// 			if (it->getName() == names[i])
// 				break;
// 		}
// 		if (it == channels.end()) {
// 			it = _server.addChannel(new newChannel(name[i]));
//     	}
// 		if (it->clients.find(current_fd) != it->clients.end()) {
// 				//erreur
// 			return false;
// 		}
// 		else if (it->password != password[i]) {
// 				//erreur
// 				return;
// 		}

// 		it->clients[fd] = &client;
// 		// Il faut un RPL_ ici
// 		// std::string message = ":" + current.nickname + "!" + current.username + "@" + current.hostname + " JOIN " + names[i] + "\r\n";
// 		// for (std::map<int, Client *>::iterator clientIt = it->clients.begin(); clientIt != it->clients.end(); ++clientIt) {
// 		// 	if (*clientIt->second != current)
// 		// 		send(clientIt->first, message.c_str(), message.length(), 0);
// 		// }
// 	}
// 	return;
// }

/********************************************************************************/