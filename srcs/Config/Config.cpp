#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ConfigException.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <set>
#include "ErrorCode.hpp"

void Config::initialize(const std::string& filepath)
{
	if (_instance != NULL)
	{
		throw ConfigException(ErrorCode::CONF_SYSTEM_ERROR, "Config instance already exists.");
	}
	_instance = new Config(filepath);
}

void Config::release()
{
	if (_instance)
	{
		delete _instance;
		_instance = NULL;
	}
}

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

Config* Config::instance()
{
	if (_instance == NULL)
	{
		throw ConfigException(ErrorCode::CONF_SYSTEM_ERROR, "Config is not initialized.");
	}
	return _instance;
}

/**
 * @brief 設定されているポートをすべて取得（ただし重複は除く）
 *
 * @return const std::vector<std::string>
 */
const std::vector<std::string> Config::getPorts()
{
	std::vector<std::string> ports;
	std::set<std::string> unique_ports;

	std::map<std::string, std::vector<ServerContext> >::const_iterator it;
	for (it = _http_block.getServers().begin(); it != _http_block.getServers().end(); ++it)
	{
		std::vector<ServerContext>::const_iterator it2;
		for (it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			// Directly add the listen value as the port, since it should only contain the port number
			unique_ports.insert(it2->getListen());
		}
	}
	ports.assign(unique_ports.begin(), unique_ports.end());
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

const std::string Config::DEFAULT_CONFIG_FILEPATH = "conf/default.conf";