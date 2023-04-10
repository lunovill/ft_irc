#include "Irc.hpp"
#include "Server.hpp"

Irc::commands const Irc::cmdList[] = {
	{"PASS", &Irc::PASS},
	{"NICK", &Irc::NICK},
	{"USER", &Irc::USER},
	{"PING", &Irc::PONG},
	{"PONG", &Irc::PING},
	{"OPER", &Irc::OPER},
	{"QUIT", &Irc::QUIT},
	{"JOIN", &Irc::JOIN},
	{"PART", &Irc::PART},
	{"MODE", &Irc::MODE},
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

void Irc::PASS(int const &fd, Client &client)
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

void	Irc::NICK(int const &fd, Client &client) {
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

void Irc::USER(int const &fd, Client &client)
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

void	Irc::OPER(int const &fd, Client &client) {
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

void	Irc::QUIT(int const &fd, Client &client) {
	_server->sendAll(fd, client, client.input);	
	return;
}

void	Irc::PART(int const &fd, Client &client) {
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

/*  La commande ping et formater comme suit: 
	PING: :1872226306
	ce nombre et token genere par le serveur la premier chose que de faire notre server c'est ping le client pour tester la connexion.

*/
void		Irc::PING(int const &fd, Client &client)
{
	std::srand(std::time(0));
	std::string token = to_string(std::rand() % 9000000000 + 1000000000);
	client.output += "PING :" + token + CLRF;
}

void		Irc::PONG(int const &fd, Client &client)
{
	std::vector<std::string> token =  to_split(client.input);
	if (token.size() != 2)
		return ;
	client.output += std::string(SERVER_NAME) + " PONG " + std::string(SERVER_NAME) + token[1] + CLRF;
}

void	Irc::JOIN(int const &fd, Client &client) {
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

/*
Mode utilisateur
Si <target> est un pseudo qui n'existe pas sur le réseau, la valeur numérique ERR_NOSUCHNICK (401) est renvoyée. 

Si <target> est un pseudo différent de celui de l'utilisateur qui a envoyé la commande, la valeur numérique ERR_USERSDONTMATCH (502) est renvoyée.

Si <modestring> n'est pas donné, le numéro RPL_UMODEIS (221) contenant les modes actuels de l'utilisateur cible est renvoyé.

Si <modestring> est donné, les modes fournis seront appliqués et un message MODE contenant les modes modifiés sera envoyé à l'utilisateur. 
Si un ou plusieurs modes envoyés ne sont pas implémentés sur le serveur, le serveur DOIT appliquer les modes qui sont implémentés, 
puis envoyer le ERR_UMODEUNKNOWNFLAG (501) en réponse avec le message MODE.

Mode canal
Si <target> est un canal qui n'existe pas sur le réseau, le message numérique ERR_NOSUCHCHANNEL (403) est renvoyé.

Si <modestring> n'est pas fourni, le message numérique RPL_CHANNELMODEIS (324) est renvoyé. 

Les serveurs PEUVENT choisir de cacher des informations sensibles telles que les clés de canal lors de l'envoi des modes actuels. 

Les serveurs PEUVENT également renvoyer le numéro RPL_CREATIONTIME (329) à la suite de RPL_CHANNELMODEIS.

Si <modestring> est indiqué, l'utilisateur qui envoie la commande DOIT disposer des privilèges appropriés sur le canal cible pour modifier les modes indiqués. 

Si un utilisateur n'a pas les privilèges appropriés pour changer de mode sur le canal cible, le serveur NE DOIT PAS traiter le message, et le code numérique ERR_CHANOPRIVSNEEDED (482) est renvoyé. 

Si l'utilisateur est autorisé à changer de mode sur la cible, les modes fournis seront appliqués en fonction du type de mode (voir ci-dessous).

Pour les modes de type A, B et C, les arguments seront obtenus séquentiellement à partir des <arguments de mode>.

Si un mode de type B ou C n'a pas de paramètre lorsqu'il est défini, le serveur DOIT ignorer ce mode.

Si un mode de type A a été envoyé sans argument, le contenu de la liste DOIT être envoyé à l'utilisateur, à moins qu'il ne contienne des informations sensibles auxquelles l'utilisateur n'est pas autorisé à accéder.

Lorsque le serveur a fini de traiter les modes, une commande MODE est envoyée à tous les membres du canal contenant les changements de mode.

Les serveurs PEUVENT choisir de masquer les informations sensibles lors de l'envoi des changements de mode.

Traduit avec www.DeepL.com/Translator (version gratuite)*/

void	Irc::MODE(int const &fd, Client &client)
{
	std::cout << "-------------------------------" << std::endl;
	std::cout << "DEBUG COMMADE TOTAL : " << client.input << std::endl;
	std::string	param = client.input.substr(5, client.input.length());
	std::vector<std::string> commands = to_split(param);
	std::string target = commands.size() == 2 ? commands[0] : "";
	std::string mode = commands.size() >= 1 ?  commands[1] : "";
	std::vector<Channel *> channel = _server->getChannels();
	std::vector<Channel *>::iterator it;

	if (target.empty())
		return ;
	try {
			if (target[0] == '#') {
				for (it = channel.begin(); it != channel.end(); ++it)
					if ((*it)->getName() == target)
						break ;
				if (it == channel.end())
					throw(1) ;
			}
			else if (target[0] != '#') {
				std::map<int, Client *>::iterator it;
				std::map<int, Client *>	clients = _server->getClients();
				for (it = clients.begin(); it != clients.end(); ++it)
				{
					if (it->second->nickname == target)
						break ;
					std::cout << "DEBUG target :" << target << " mode: " << mode << std::endl;
					std::cout << "-------------------------------" << std::endl;

				}
				if (it == clients.end())
					throw(2);
				else if (target != client.nickname)
					throw(3);
			}

			if (mode.empty() && target[0] == '#')
				throw(4) ; 
			else if (mode.empty() && target[0] != '#')
				throw(5) ;
			else if (target[0] == '#' && strchr(client.mode.c_str(), 'o'))
					throw(7);

			int len = mode.length();
			for (int i = 0; i < len; i++)
			{
				while (i < len && (mode[i] == '+' || mode[i] == '-'))
				{
					char signe = mode[i]; 
					i++;
					if (!strchr("+-", mode[i]))
					{
						if (target[0] != '#') 
						{
							if (signe == '+' && strchr(USER_MODES, mode[i]) && !strchr(client.mode.c_str(), mode[i]))
								client.mode += mode[i];
							else if (signe == '-' && strchr(USER_MODES, mode[i]))
								client.mode.erase(mode[i]);
							else if (!strchr(USER_MODES, mode[i]))
								throw(6);
						}
						else if (target[0] == '#')
						{
							if (signe == '+' && strchr(CHANNEL_MODES, mode[i]) && !strchr((*it)->getMode().c_str(), mode[i]))
								(*it)->setMode(mode[i]);
							else if (signe == '-' && strchr(CHANNEL_MODES, mode[i]))
								(*it)->unsetMode(mode[i]);	
						}
					}
				}
				i++;
			}
			target[0] == '#' ? client.output += SERVER_NAME + std::string("+") + (*it)->getMode() + CLRF : client.output += SERVER_NAME + std::string("+") + client.mode + CLRF;
			return ;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	}
	catch (int e) {
		switch (e) {
			case 1:
				client.output += ERR_NOSUCHCHANNEL(target);
				break ;
			case 2:
				client.output +=  ERR_NOSUCHNICK(target);
				break ;
			case 3:
				client.output += ERR_USERSDONTMATCH(target);
				break ;
			case 4:
				client.output += RPL_CHANNELMODEIS(target, (*it)->getMode());
				break ;
			case 5:
				client.output +=  RPL_UMODEIS(target, "");
				break ;
			case 6:
				client.output += ERR_UMODEUNKNOWNFLAG(target);
				break ;
			case 7:
				client.output += ERR_CHANOPRIVSNEEDED((*it)->getName());
				break ;
		}
	}
	return ;
}

/********************************************************************************/
