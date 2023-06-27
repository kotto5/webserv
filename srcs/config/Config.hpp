#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include "ServerConfig.hpp"

class Config
{
    public:
        Config();
        ~Config();
        void readFile(const std::string& filepath);
        void addServer(const ServerConfig& server_config);
        const std::map<int, std::vector<ServerConfig> >& getServers() const;

    private:
        std::map<int, std::vector<ServerConfig> > _servers;
};

#endif