#include <cstdlib>
#include "Server.hpp"

int main(int argc, char *argv[]) {
    if (argc != 3)
        return 1;
    Server irc(std::atoi(argv[1]), argv[2]);
    irc.run();
    return 0;
}
