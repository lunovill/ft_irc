#pragma once

# include <iostream>

class Client {

    // private:

    //     int _fd;

    public:
        Client(void);
        ~Client(void);

        bool        cmd_register[4];                                               
        std::string nickname;
        std::string username;
        std::string hostname;
        std::string realname;

        int        is_register(void);

        // std::string                                         password;
        // std::vector<std::string>                            channel
};
