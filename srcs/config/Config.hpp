#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include "ServerContext.hpp"

class Config
{
    public:
        Config();
        ~Config();
        void readFile(const std::string& filepath);
        void addServer(const ServerContext& server_config);
        const std::map<int, std::vector<ServerContext> >& getServers() const;

    private:
        std::map<int, std::vector<ServerContext> > _servers;
};

#endif