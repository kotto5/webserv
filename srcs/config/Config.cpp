#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"

Config::Config(const std::string& filepath)
{
	ConfigParser parser(*this);
    parser.parseFile(filepath);
}

Config::~Config()
{
}

HTTPContext& Config::getHTTPBlock()
{
	return _http_block;
}

Config *Config::getInstance()
{
	if (_instance == NULL)
	{
		_instance = new Config("conf/default.conf");
	}
	return _instance;
}

Config *Config::_instance = NULL;