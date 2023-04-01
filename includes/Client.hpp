#pragma once

# include <iostream>

class Client {

    // private:

    //     int _fd;

    public:
        Client(int fd);
        ~Client(void);

        int         fd;
        bool        cmd_register[4];                                               
        std::string nickname;
        std::string username;
        std::string hostname;
        std::string realname;

        int        is_register(void);
         bool operator!=(const Client& other) const;
         bool operator==(const Client& other) const;

        // std::string                                         password;
        // std::vector<std::string>                            channel
};
