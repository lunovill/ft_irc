#include "Channel.hpp" 

/********************************************************************************/
/* ------------------------------- CONSTRUCTOR -------------------------------- */
/********************************************************************************/

Channel::Channel(std::string name) : _name(name), _password(NULL) { return; }

/********************************************************************************/
/* -------------------------------- DESTRUCTOR -------------------------------- */
/********************************************************************************/

Channel::Channel(std::string name, std::string password) : _name(name), _password(password) { return; }

Channel::~Channel(void) { return; }

/********************************************************************************/

std::string Channel::getName(void) const { return _name; }
std::string Channel::getMode(void) const { return _mode; }
void        Channel::setMode(std::string mode) { _mode = mode; }