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
	{"QUIT", &Irc::QUIT},
	{"OPER", &Irc::OPER},
	{"JOIN", &Irc::JOIN},
	{"PART", &Irc::PART},
	// {"LIST", &LIST},
	// {"EXIT", &EXIT},
	// {"PRIVMSG", &PRIVMSG},
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
	if (_server->findClientNick(nickname)) {
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
		return;
	}
	if (client.nickname.empty())
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
	if (client.mode.find('o') == std::string::npos)
		client.mode += "o";
	client.output += RPL_YOUREOPER(client.nickname);
	return;
}

void	Irc::QUIT(int fd, Client &client) {
	_server->sendAll(fd, client, client.input);	
	return;
}

void	Irc::PART(int fd, Client &client) {
	std::vector<std::string>	input = to_split(client.input.substr(5, client.input.length() - 5));
	if (input.size() < 1) {
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "OPER");
		return;
	}
	Channel *channel = _server->getChannel(input[0]);
	if (!channel) {
		client.output += ERR_NOSUCHCHANNEL(input[0]);
		return;
	} else if (!channel->findClient(fd)) {
		client.output += ERR_NOTONCHANNEL(client.nickname, input[0]);
		return;
	}
	if (!channel->eraseClient(fd))
		_server->eraseChannel(channel);
	client.output += std::string(":") + client.nickname + std::string("!~u@") + client.hostname + std::string(".irc PART ") + input[0] + CLRF;
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
	Channel *channel;;
	for (unsigned int i = 0; i < names.size(); i++) {
		channel = _server->getChannel(names[i]);
		if (!channel) {
			if (input.size() == 2 && i < passwords.size()) channel = new Channel(names[i], passwords[i]);
			else channel = new Channel(names[i]);
			_server->addChannel(channel);
    	} else if (channel->getMode().find('k') != std::string::npos && (i >= passwords.size()) || (i < passwords.size() && channel->getPass() != passwords[i])) {
			client.output += ERR_BADCHANNELKEY(client.nickname, names[i]);
			break;
		}
		if (!channel->addClient(fd, client)) { // A verifier quand on fera le mode +l

			client.output += ERR_CHANNELISFULL(client.nickname, names[i]);
			break;
		}

		client.output += std::string(":") + client.nickname + std::string("!~u@") + client.hostname + std::string(".irc JOIN ") + input[0] + CLRF;
		if (!channel->topic.empty())
			client.output += RPL_TOPIC(client.nickname, names[i], channel->topic);
		client.output += RPL_NAMREPLY(client.nickname, names[i], channel->clientList(client.nickname));
		client.output += RPL_ENDOFNAMES(client.nickname, names[i]);
	}
	return;
}
