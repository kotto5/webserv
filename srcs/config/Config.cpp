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

const std::vector<int>& Config::getPorts()
{
	std::vector<int> ports;

	for (std::map<std::string, std::vector<ServerContext> >::const_iterator it = _http_block.getServers().begin();
			it != _http_block.getServers().end(); ++it)
	{
		for (std::vector<ServerContext>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			std::string listen = it2->getListen();
			std::string::size_type pos = listen.find(':');
			if (pos != std::string::npos)
			{
				std::string port = listen.substr(pos + 1);
				ports.push_back(atoi(port.c_str()));
			}
		}
	}
	return ports;
}

Config* Config::_instance = NULL;