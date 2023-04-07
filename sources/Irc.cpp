#include "Irc.hpp"
#include "Utils.hpp"
#include "Server.hpp"

const	Irc::commands Irc::cmdList[] = {
	{"PASS", &Irc::PASS},
	{"NICK", &Irc::NICK},
	{"USER", &Irc::USER},
	{"MODE", &Irc::MODE},
	{"PING", &Irc::PONG},
	{"PONG", &Irc::PING},
	{"OPER", &Irc::OPER},
	{"JOIN", &Irc::JOIN},
	// {"LIST", &LIST},
	// {"EXIT", &EXIT},
	// {"PRIVMSG", &PRIVMSG},
	// {"PART", &PART},
	// {"TOPIC", &TOPIC},
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
			return;
		}
	client.nickname = nickname;
	if (client.cmdRegister[1] != true)
		client.cmdRegister[1] = true;	
	return;
}

void Irc::USER(int fd, Client &client)
{
	if (client.cmdRegister[3] == true) {
		client.output += ERR_ALREADYREGISTRED(client.nickname);
		return;
	}
	std::vector<std::string> input = to_split(client.input.substr(5, client.input.length() - 5));
	if (input.size() < 5)
	{
		client.output += ERR_NEEDMOREPARAMS(client.nickname, input[0]);
		send(fd, client.output.c_str(), client.output.length(), 0);
		return;
	}
	client.nickname = input[0];
	client.username = input[1];
	client.hostname = input[2];
	client.realname = input[3] + " " + input[4];
	if (client.cmdRegister[2] != true)
		client.cmdRegister[2] = true;
}

void	Irc::OPER(int fd, Client &client) {
	std::vector<std::string>	input = to_split(client.input.substr(5, client.input.length() - 5));
	if (input.size() < 2) {
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "OPER");
		return;
	}
	std::string	username = input[0];
	std::string	password = input[1];
	if (password != "carotte") {
		client.output += ERR_PASSWDMISMATCH(client.nickname);
		return;
	}
	if ((username != "lunovill" && username != "engooh" && username != "sredjini") || client.hostname != "localhost") {
		client.output += ERR_NOOPERHOST(client.nickname);
		return;
	}
	client.mode += "o";
	client.output += RPL_YOUREOPER(client.nickname);
	return;
}

void	Irc::MODE(int fd, Client &client)
{
	std::vector<std::string> commands = to_split(client.input);
	std::string target = commands.size() == 2 ? commands[1] : "";
	std::string mode = commands.size() == 3 ?  commands[2] : "";

	// if (target[0] == '#')
	// {
	// 	std::vector<Channel *> channel = _server->getChannel();
	// 	for (std::vector<Channel *>::iterator it = channel.begin(); it != channel.end(); ++it)
	// 	{
	// 		if ((*it)->getName() == target)
	// 			break ;
	// 		else if (it == channel.end())
	// 		{
	// 			client.output += ERR_NOSUCHCHANNEL(target);
	// 			return ;
	// 		}
	// 	}
	// }
	// else
	// {
	// 	std::map<int, Client *> clients = _server->getClients();
	// 	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	// 	{
	// 		if (it->second->nickname == target)
	// 			break ;
	// 		if (it == clients.end()) 
	// 		{
	// 			client.output += ERR_NOSUCHNICK(target);
	// 			return ;
	// 		}
	// 	}
	// 	if (target != client.username)
	// 	{
	// 		client.output += ERR_USERSDONTMATCH(target);
	// 		return ;
	// 	}
	// }
	// if (mode.empty())
	// {
		if (target[0] == '#')
			client.output += RPL_CHANNELMODEIS(target, mode);
		else 
			client.output += RPL_UMODEIS(client.nickname, mode);
		return ;
	// }
	// else
	// {
	// 		for (int i = 0; i < mode.size(); i++)
	// 		{
	// 			if (target[0] != '#' || (mode[i] != '-' && mode[i] != '+' && mode[i] == 'i' && mode[i] != 'o' && mode[i] != 'b'))
	// 			{
	// 				client.output += ERR_UMODEUNKNOWNFLAG(target);
	// 				return ;
	// 			}
	// 			else if (target[0] == '#')
	// 			{
	// 					// implementer les mode ...
	// 			}
	// 		}
	// }
}

/*  La commande ping et formater comme suit: 
	PING: :1872226306
	ce nombre et token genere par le serveur la premier chose que de faire notre server c'est ping le client pour tester la connexion.

*/
void		Irc::PING(int fd, Client &client)
{
	std::srand(std::time(0));
	std::string token = to_string(std::rand() % 9000000000 + 1000000000);
	client.output += "PING :" + token + CLRF;
}

void		Irc::PONG(int fd, Client &client)
{
	std::vector<std::string> token =  to_split(client.input);
	if (token.size() != 2)
		return ;
	client.output += std::string(SERVER_NAME) + " PONG " + std::string(SERVER_NAME) + token[1] + CLRF;
}

void	Irc::JOIN(int fd, Client &client) {
	std::vector<std::string>	input = to_split(client.input.substr(5, client.input.length() - 5));
	if (input.empty()) {
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "JOIN");
		return;
	}

	std::vector<std::string>	names = to_split(input[0], ',');
	std::vector<std::string>	passwords;
	if (input.size() > 1) passwords = to_split(input[1], ',');
	std::vector<Channel *>		channels = _server->getChannels();
	std::vector<Channel *>::iterator it;
	for (unsigned int i = 0; i < names.size(); i++) {
		for (it = channels.begin(); it != channels.end(); ++it) {
			if ((*it)->getName() == names[i])
				break;
		}
		if (it == channels.end()) {
			Channel *newChannel;
			if (input.size() == 2 && i < passwords.size()) newChannel = new Channel(names[i], passwords[i]);
			else newChannel = new Channel(names[i]);
			it = _server->addChannel(newChannel);
    	}
		else if ((*it)->getMode().find('k') != std::string::npos && (i >= passwords.size()) || (i < passwords.size() && (*it)->getPass() != passwords[i])) {
			client.output += ERR_BADCHANNELKEY(client.nickname, names[i]);
			// PART
			break;
		}
		if (!(*it)->addClient(fd, client)) { // A verifier quand on fera le mode +l
			client.output += ERR_CHANNELISFULL(client.nickname, names[i]);
			// PART
			break;
		}

		if (!(*it)->topic.empty())
			client.output += RPL_TOPIC(client.nickname, names[i], (*it)->topic);
		std::string clientsNames = client.nickname;
		for (std::map<int, Client *>::iterator itClient = (*it)->clients.begin(); itClient != (*it)->clients.end(); ++itClient)
			if (itClient->first != fd)
				clientsNames += " " + itClient->second->nickname;
		client.output += RPL_NAMREPLY(client.nickname, names[i], clientsNames);
		client.output += RPL_ENDOFNAMES(client.nickname, names[i]);
	}
	return;
}
