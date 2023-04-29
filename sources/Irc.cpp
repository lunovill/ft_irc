#include "Irc.hpp"
#include "Server.hpp"

Irc::commands const Irc::cmdList[] = {
	{"PASS", &Irc::PASS},
	{"NICK", &Irc::NICK},
	{"USER", &Irc::USER},
	{"userhost", &Irc::USER},
	{"PING", &Irc::PONG},
	{"PONG", &Irc::PING},
	{"OPER", &Irc::OPER},
	{"QUIT", &Irc::QUIT},
	{"JOIN", &Irc::JOIN},
	{"PART", &Irc::PART},
	{"TOPIC", &Irc::TOPIC},
	{"KICK", &Irc::KICK},
	{"MODE", &Irc::MODE},
	{"PRIVMSG", &Irc::PRIVMSG},
	{"NOTICE", &Irc::NOTICE},
	{"kill", &Irc::KILL},
	{"wallops", &Irc::WALLOPS},
	{"", NULL}};

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

Irc::CommandFt Irc::find(std::string const &input) const
{
	for (unsigned int i = 0; !Irc::cmdList[i].name.empty(); ++i)
	{
		std::string cmdName(Irc::cmdList[i].name);
		if (input.compare(0, cmdName.length(), cmdName) == 0 && (input[cmdName.length()] == ' ' || input[cmdName.length()] == '\r' || input[cmdName.length()] == '\n'))
			return Irc::cmdList[i].cmd;
	}
	return NULL;
};

void Irc::PASS(int const &fd, Client &client)
{
	(void)fd;
	if (client.cmdRegister[3] == true)
	{
		client.output += ERR_ALREADYREGISTRED(client.nickname);
		return;
	}
	std::string password = client.input.substr(5, client.input.length() - 5);
	if (password.empty())
	{
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "PASS");
		return;
	}
	if (password != _server->getPass())
	{
		client.output += ERR_PASSWDMISMATCH(client.nickname);
		return;
	}
	if (client.cmdRegister[0] != true)
		client.cmdRegister[0] = true;
	return;
}

void Irc::NICK(int const &fd, Client &client)
{
	(void)fd;
	std::string nickname = client.input.substr(5, client.input.length() - 5);
	if (nickname.empty())
	{
		client.output += ERR_NONICKNAMEGIVEN(client.nickname);
		return;
	}
	for (unsigned int i = 0; i < nickname.length(); ++i)
		if (!std::isalnum(nickname[i]) && nickname[i] != '-' && nickname[i] != '_')
		{
			client.output += ERR_ERRONEUSNICKNAME(client.nickname, nickname);
			return;
		}
	if (nickname == "bot" || _server->findClient(nickname))
	{
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
	(void)fd;
	if (client.cmdRegister[3] == true)
	{
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
	client.realname = input[3].substr(1) + " " + input[4];
	if (client.cmdRegister[2] != true)
		client.cmdRegister[2] = true;
}

void Irc::OPER(int const &fd, Client &client)
{
	(void)fd;
	std::vector<std::string> input = to_split(client.input.substr(5, client.input.length() - 5));
	if (input.size() < 2)
	{
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "OPER");
		return;
	}
	std::string username = input[0];
	std::string password = input[1];

	if (password != "carotte")
	{
		client.output += ERR_PASSWDMISMATCH(client.nickname);
		return;
	}
	if ((username != "lunovill" && username != "engooh" && username != "sredjini") || client.hostname != "localhost")
	{
		client.output += ERR_NOOPERHOST(client.nickname);
		return;
	}
	if (client.mode.find('o') == std::string::npos)
		client.mode += "o";
	client.output += RPL_YOUREOPER(client.nickname);

	return;
}

void Irc::QUIT(int const &fd, Client &client)
{
	_server->sendAll(fd, client, client.input, false);
	return;
}

void Irc::PING(int const &fd, Client &client)
{
	(void)fd;
	std::srand(std::time(0));
	std::string token = to_string(std::rand() % 9000000000 + 1000000000);
	client.output += "PING :" + token + CLRF;
}

void Irc::PONG(int const &fd, Client &client)
{
	(void)fd;
	std::vector<std::string> token = to_split(client.input);
	if (token.size() != 2)
		return;
	client.output += std::string(SERVER_NAME) + " PONG " + std::string(SERVER_NAME) + token[1] + CLRF;
}

void Irc::JOIN(int const &fd, Client &client)
{
	std::vector<std::string> input = to_split(client.input.substr(5, client.input.length() - 5));
	if (input.empty())
	{
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "JOIN");
		return;
	}

	std::vector<std::string> names = to_split(input[0], ',');
	std::vector<std::string> passwords;
	if (input.size() > 1)
		passwords = to_split(input[1], ',');
	Channel *channel;
	for (unsigned int i = 0; i < names.size(); i++)
	{
		channel = _server->getChannel(names[i]);
		if (!channel)
		{
			if (input.size() == 2 && i < passwords.size())
				channel = new Channel(names[i], passwords[i]);
			else
				channel = new Channel(names[i]);
			_server->addChannel(channel);
		}
		else if (channel->getMode().find('k') != std::string::npos && (i >= passwords.size() || (i < passwords.size() && channel->getPass() != passwords[i])))
		{
			client.output += ERR_BADCHANNELKEY(client.nickname, names[i]);
			break;
		}
		if (!channel->addClient(fd, client))
		{
			client.output += ERR_CHANNELISFULL(client.nickname, names[i]);
			break;
		}

		client.output += std::string(":") + client.nickname + std::string("!~u@") + client.hostname + std::string(".irc ") + client.output + CLRF;
		if (!channel->topic.empty())
			client.output += RPL_TOPIC(client.nickname, names[i], channel->topic);
		else
			client.output += RPL_NOTOPIC(client.nickname, names[i]);
		client.output += RPL_NAMREPLY(client.nickname, names[i], channel->clientList(client.nickname));
		client.output += RPL_ENDOFNAMES(client.nickname, names[i]);
	}
	return;
}

void Irc::PART(int const &fd, Client &client)
{
	std::vector<std::string> input = to_split(client.input.substr(5, client.input.length() - 5));
	if (input.size() < 1)
	{
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "PART");
		return;
	}
	Channel *channel = _server->getChannel(input[0]);
	if (!channel)
	{
		client.output += ERR_NOSUCHCHANNEL(client.nickname, input[0]);
		return;
	}
	else if (!channel->findClient(fd))
	{
		client.output += ERR_NOTONCHANNEL(client.nickname, input[0]);
		return;
	}
	if (!channel->eraseClient(fd))
		_server->eraseChannel(channel);
	channel->sendAll(fd, client, client.input, false);
	client.output += std::string(":") + client.nickname + std::string("!~u@") + client.hostname + std::string(".irc PART ") + input[0] + CLRF;
	return;
}
void Irc::TOPIC(int const &fd, Client &client)
{
	std::string input = client.input.substr(6, client.input.length() - 6);
	if (input.empty())
	{
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "TOPIC");
		return;
	}
	Channel *channel = _server->getChannel(input.substr(0, input.find(' ')));
	if (!channel)
	{
		client.output += ERR_NOSUCHCHANNEL(client.nickname, input.substr(0, input.find(' ')));
		return;
	}
	else if (!channel->findClient(fd))
	{
		client.output += ERR_NOTONCHANNEL(client.nickname, channel->getName());
		return;
	}
	else if (channel->getMode().find('t') != std::string::npos && client.mode.find('o') == std::string::npos)
	{
		client.output += ERR_CHANOPRIVSNEEDED(channel->getName());
		return;
	}
	channel->topic = input.substr(channel->getName().length() + 2, input.length() - (channel->getName().length()) + 2);
	client.output += std::string(":") + client.nickname + std::string("!~u@") + client.hostname + std::string(".irc ") + client.input + CLRF;
	channel->sendAll(fd, client, client.input, false);
}

void Irc::KICK(int const &fd, Client &client)
{
	std::vector<std::string> input = to_split(client.input.substr(5, client.input.length() - 5));
	if (input.size() < 2)
	{
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "KICK");
		return;
	}
	Channel *channel = _server->getChannel(input[0]);
	if (!channel)
	{
		client.output += ERR_NOSUCHCHANNEL(client.nickname, input[0]);
		return;
	}
	else if (client.mode.find("o") == std::string::npos)
	{
		client.output += ERR_CHANOPRIVSNEEDED(input[0]);
		return;
	}
	else if (!channel->findClient(fd))
	{
		client.output += ERR_NOTONCHANNEL(client.nickname, input[0]);
		return;
	}
	else if (!channel->findClient(input[1]))
	{
		client.output += ERR_USERNOTINCHANNEL(client.nickname, input[1], input[0]);
		return;
	}
	if (!channel->eraseClient(fd))
		_server->eraseChannel(channel);
	_server->sendClient(client, input[1], client.input, false);
	return;
}

void Irc::MODE(int const &fd, Client &client)
{
	(void)fd;
	std::string param = client.input.substr(5, client.input.length());
	std::vector<std::string> commands = to_split(param);
	std::string target = commands.size() >= 1 ? commands[0] : "";
	std::string mode = commands.size() >= 2 ? commands[1] : "";
	std::vector<std::string> set;
	if (commands.size() >= 3)
		set = to_split(commands[2], ',');
	Channel *channel;

	if (target.empty()) { return; }
	else if (target[0] == '#') {
		channel = _server->getChannel(target);
		if (!channel) {
			client.output += ERR_NOSUCHCHANNEL("MODE", target);
			return;
		}
	} else if (target[0] != '#') {
		if (!_server->findClient(target) || client.nickname != target) {
			client.output += ERR_NOSUCHNICK(target, "");
			return;
		} else if (target != client.nickname) {
			client.output += ERR_USERSDONTMATCH(target);
			return;
		}
	}
	if (mode.empty() && target[0] == '#') {
		client.output += RPL_CHANNELMODEIS(target, channel->getMode());
		return;
	} else if (mode.empty() && target[0] != '#') {
		client.output += RPL_UMODEIS(target, "");
		return;
	} else if (target[0] == '#' && !strchr(client.mode.c_str(), 'o')) {
		client.output += ERR_CHANOPRIVSNEEDED(channel->getName());
		return;
	}

	char signe;
	unsigned int j = -1;
	for (unsigned int i = 0; i < mode.length(); ++i) {
		if (mode[i] == 'k' || mode[i] == 'l' || mode[i] == 'o') { j++; }
		if (mode[i] == '+' || mode[i] == '-') { signe = mode[i]; }
		else if (!strchr("+-", mode[i])) {
			if (target[0] != '#') {
				if (signe == '+' && strchr(USER_MODES, mode[i]) && !strchr(client.mode.c_str(), mode[i])) {
					if ((mode[i] == 'o' && !set.empty()) && set.size() > j) {
						client.input = "OPER " + target + " " + set[j];
						this->OPER(fd, client);
					} else if (mode[i] != 'o') {
						client.mode += mode[i];
					}
				}
				else if (signe == '-' && strchr(USER_MODES, mode[i])) {
					if (!client.mode.empty() && client.mode.find(mode[i]) != std::string::npos) { client.mode.erase(client.mode.find(mode[i]), 1); }
				} else if (!strchr(USER_MODES, mode[i])) {
					client.output += ERR_UMODEUNKNOWNFLAG(target);
					return;
				}
			} else if (target[0] == '#') {
				if (signe == '+' && strchr(CHANNEL_MODES, mode[i]) && !strchr(channel->getMode().c_str(), mode[i])) {
					if ((mode[i] == 'k' && !set.empty()) && set.size() > j) {
						channel->setMode(mode[i]);
						channel->setPass(set[j]);
					} else if ((mode[i] == 'l' && !set.empty()) && set.size() > j) {
						for (const char *c = set[j].c_str(); *c != '\0'; ++c)
							if (!isdigit(*c)) {	continue; }
						channel->setClientLimit(set[j]);
						channel->setMode(mode[i]);
					}
				} else if (signe == '-' && strchr(CHANNEL_MODES, mode[i])) {
					channel->eraseMode(mode[i]);
				}
			}
		}
	}
	(target[0] == '#') ? client.output += SERVER_NAME + std::string(" ") + std::string("+") + channel->getMode() + CLRF : client.output += SERVER_NAME + std::string(" ") + std::string("+") + client.mode + CLRF;
	return;
}

void Irc::PRIVMSG(int const &fd, Client &client)
{
	std::string input = client.input.substr(8, client.input.length() - 8);
	if (input.empty())
	{
		client.output += ERR_NORECIPIENT(client.nickname, "PRIVMSG");
		return;
	}
	else if (input.find(' ') == std::string::npos)
	{
		client.output += ERR_NOTEXTTOSEND(client.nickname);
		return;
	}
	std::string recever = input.substr(0, input.find(' '));
	bool oper = (recever[0] == '@') ? true : false;

	if (oper)
		recever = recever.substr(1);
	if (recever[0] != '#')
	{
		if (recever.find(',') != std::string::npos)
		{
			client.output += ERR_TOOMANYTARGETS(client.nickname);
			return;
		}
		else if (!_server->findClient(recever))
		{
			client.output += ERR_NOSUCHNICK(client.nickname, recever);
			return;
		}
		_server->sendClient(client, recever, client.input, oper);
	}
	else
	{
		Channel *channel = _server->getChannel(recever);
		if (!channel || (channel->getMode().find('n') != std::string::npos && !channel->findClient(fd)))
		{
			client.output += ERR_CANNOTSENDTOCHAN(client.nickname, recever);
			return;
		}
		channel->sendAll(fd, client, client.input, oper);
	}
	return;
}

void Irc::KILL(int const &fd, Client &client)
{
	(void)fd;
	std::vector<std::string> input = to_split(client.input.substr(5, client.input.length() - 5));
	if (input.size() < 1)
	{
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "KILL");
		return;
	}
	else if (client.mode.find("o") == std::string::npos)
	{
		client.output += ERR_NOPRIVILEGES(client.nickname);
		return;
	}
	else if (!_server->findClient(input[0]))
	{
		client.output += ERR_NOSUCHNICK(client.nickname, input[0]);
		return;
	}
	_server->sendClient(client, input[0], client.input, false);
	_server->desconnectClient(input[0]);
	return;
}

void Irc::WALLOPS(int const &fd, Client &client)
{
	std::vector<std::string> input = to_split(client.input.substr(8, client.input.length() - 8));
	if (input.size() < 2)
	{
		client.output += ERR_NEEDMOREPARAMS(client.nickname, "WALLOPS");
		return;
	}
	else if (client.mode.find("o") == std::string::npos)
	{
		client.output += ERR_NOPRIVILEGES(client.nickname);
		return;
	}
	_server->sendAll(fd, client, client.input, true);
	return;
}

void Irc::NOTICE(int const &fd, Client &client)
{
	std::string input = client.input.substr(7, client.input.length() - 7);
	if (input.empty()) { return; }
	else if (input.find(' ') == std::string::npos) { return; }
	std::string recever = input.substr(0, input.find(' '));
	bool oper = (recever[0] == '@') ? true : false;

	if (oper)
		recever = recever.substr(1);
	if (recever[0] != '#')
	{
		if (recever.find(',') != std::string::npos) { return; }
		else if (recever == "bot") {
			std::string set = input.substr(5);
			if (set == "JOKE" || set == "INFO") { bot(client, set); }
		} else if (!_server->findClient(recever)) { return; }
		_server->sendClient(client, recever, client.input, oper);
	}
	else
	{
		Channel *channel = _server->getChannel(recever);
		if (!channel || (channel->getMode().find('n') != std::string::npos && !channel->findClient(fd))) { return; }

		channel->sendAll(fd, client, client.input, oper);
	}
	return;
}

void Irc::bot(Client &client, std::string const &set)
{
	if (_jokes.empty()) {
		std::string line;
		std::ifstream file("./utils/JOKE.txt");

		if (!file.is_open())
			return;
		while (std::getline(file, line))
			_jokes.push_back(line);
		file.close();
	}
	if (_infos.empty()) {
		std::string line;
		std::ifstream file("./utils/INFO.txt");

		if (!file.is_open())
			return;
		while (std::getline(file, line))
			_infos.push_back(line);
		file.close();
	}

	if (set == "JOKE") {
		std::srand(std::time(NULL));
		int index = std::rand() % _jokes.size();
		client.output = std::string(":") + "bot" + std::string("!~u@") + client.hostname + std::string(".irc PRIVMSG ") + client.nickname + std::string(" :") + _jokes[index] + CLRF;
	}
	if (set == "INFO") {
		std::srand(std::time(NULL));
		int index = std::rand() % _infos.size();
		client.output = std::string(":") + "bot" + std::string("!~u@") + client.hostname + std::string(".irc PRIVMSG ") + client.nickname + std::string(" :") + _infos[index] + CLRF;
	}
	return;
}
