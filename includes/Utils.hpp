#pragma once 

# include <cstdlib>
# include <ctime>
# include <vector>
# include <string>
# include <sstream>
# include <iostream>

/* cette fonction est t'une fonction utilitaire qui sert a convertir des int, long, float, double bref tout les type numerique en std::string*/
template<typename T>
std::string to_string(T param)
{
  std::ostringstream oss;
  oss << param;
  std::string str = oss.str();
  return str;
}

template<typename T>
std::vector<std::string> to_split(T msg, char c = ' ')
{

	std::string line;
	std::vector<std::string> vec;
	std::istringstream stream(msg);

	while (std::getline(stream, line, c))
		if (!line.empty())
			vec.push_back(line);
	return vec;
}