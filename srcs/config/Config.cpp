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

Config* Config::_instance = NULL;