#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"
#include <map>

Config::Config()
{
}

Config::~Config()
{
}

void Config::readFile(const std::string& filepath)
{
    ConfigParser parser(*this);
    parser.parseFile(filepath);
}

const std::map<int, std::vector<ServerContext> >& Config::getServers() const
{
    return _servers;
}

void Config::addServer(const ServerContext& server_context)
{
    int listen = server_context.getListen();
    std::map<int, std::vector<ServerContext> >::iterator
        port_found = _servers.find(listen);

    if (port_found != _servers.end())
    {
        if (server_context.getServerName().empty())
        {
            //exception
        }
        std::vector<ServerContext> &servers = port_found->second;
        for (size_t i = 0; i < servers.size(); i++)
        {
            if (servers.at(i).getServerName() == server_context.getServerName())
            {
                //exception
            }
        }
        servers.push_back(server_context);
    }
    else
    {
        std::vector<ServerContext> new_servers(1, server_context);
        _servers.insert(std::make_pair(listen, new_servers));
    }
}
