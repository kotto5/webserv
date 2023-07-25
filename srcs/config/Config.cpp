#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ConfigError.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>

Config::Config(const std::string& filepath)
{
	ConfigParser parser(*this);
	parser.parseFile(filepath);
	_instance = this;
}

HTTPContext& Config::getHTTPBlock()
{
	return _http_block;
}

Config* Config::getInstance()
{
	return _instance;
}

/**
 * @brief 設定されているポートをすべて取得
 *
 * @return const std::vector<std::string>
 */
const std::vector<std::string> Config::getPorts()
{
	std::vector<std::string> ports;

	std::map<std::string, std::vector<ServerContext> >::const_iterator it;
	for (it = _http_block.getServers().begin(); it != _http_block.getServers().end(); ++it)
	{
		std::vector<ServerContext>::const_iterator it2;
		for (it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			// Directly add the listen value as the port, since it should only contain the port number
			ports.push_back(it2->getListen());
		}
	}
	return ports;
}

// シングルトンパターンのため外部からの変更・破棄を避ける
Config::~Config()
{
}

Config::Config(const Config& other)
{
	*this = other;
}

Config& Config::operator=(const Config& other)
{
	if (this != &other)
	{
		_http_block = other._http_block;
	}
	return *this;
}

Config* Config::_instance = NULL;