#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"

Config::Config(const std::string& filepath)
{
	ConfigParser parser(*this);
    parser.parseFile(filepath);
	_instance = this;
}

Config::~Config()
{
}

HTTPContext& Config::getHTTPBlock()
{
	return _http_block;
}

Config* Config::getInstance()
{
	return _instance;
}

const std::vector<std::string>& Config::getPorts()
{
	std::vector<std::string> ports;

	for (std::map<std::string, std::vector<ServerContext> >::const_iterator it = _http_block.getServers().begin();
			it != _http_block.getServers().end(); ++it)
	{
		for (std::vector<ServerContext>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			// Directly add the listen value as the port, since it should only contain the port number
			ports.push_back(it2->getListen());
		}
	}
	return ports;
}

Config* Config::_instance = NULL;