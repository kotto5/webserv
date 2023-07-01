#include "HTTPContext.hpp"

HTTPContext::HTTPContext():
	_accessLogFile(),
	_errorLogFile()
{
}

HTTPContext::~HTTPContext()
{
}

void HTTPContext::setAccessLogFile(const std::string& accessLogFile)
{
	_accessLogFile = accessLogFile;
}

void HTTPContext::setErrorLogFile(const std::string& errorLogFile)
{
	_errorLogFile = errorLogFile;
}

const std::string& HTTPContext::getAccessLogFile() const
{
	return _accessLogFile;
}

const std::string& HTTPContext::getErrorLogFile() const
{
	return _errorLogFile;
}

const std::map<int, std::vector<ServerContext> >& HTTPContext::getServers() const
{
    return _servers;
}

void HTTPContext::addServerBlock(const ServerContext& server)
{
    int listen = server.getListen();
    std::map<int, std::vector<ServerContext> >::iterator
        port_found = _servers.find(listen);

    if (port_found != _servers.end())
    {
        if (server.getServerName().empty())
        {
            //exception

        }
        std::vector<ServerContext> &servers = port_found->second;
        for (size_t i = 0; i < servers.size(); i++)
        {
            if (servers.at(i).getServerName() == server.getServerName())
            {
                //exception
            }
        }
        servers.push_back(server);
    }
    else
    {
        std::vector<ServerContext> new_servers(1, server);
        _servers.insert(std::make_pair(listen, new_servers));
    }
}
