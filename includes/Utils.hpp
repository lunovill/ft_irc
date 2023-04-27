#pragma once 

# include <cstdlib>
# include <ctime>
# include <vector>
# include <string>
# include <sstream>
# include <iostream>

template<typename T>
std::string to_string(T param)
{
  std::ostringstream oss;
  oss << param;
  std::string str = oss.str();
  return str;
}

template<typename T>
size_t to_sizeT(T str)
{
	std::istringstream iss(str);
    size_t size;
    iss >> size;
	return size;
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