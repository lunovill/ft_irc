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