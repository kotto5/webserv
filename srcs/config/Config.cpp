#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ConfigError.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"
#include <unistd.h>
#include <fcntl.h>
#include "Error.hpp"

Config::Config(const std::string& filepath)
{
    ConfigParser parser(*this);
    parser.parseFile(filepath);
    _instance = this;

	std::string errorLogFile = getHTTPBlock().getErrorLogFile();
	setErrorLogFileStderror(errorLogFile);
}

int	Config::setErrorLogFileStderror(std::string errorLogFile)
{
	if (errorLogFile.empty())
		return (0);
	if (!errorLogFile.empty())
	{
		int logFile;
		logFile = open(errorLogFile.c_str(), O_RDWR | O_CREAT | O_APPEND, 0666);
		if (logFile > 0)
		{
			if (dup2(logFile, STDERR_FILENO) == -1)
				throw (ConfigError(SYSTEM_ERROR, "dup2"));
		}
		else
			throw (ConfigError(SYSTEM_ERROR, "open"));
	}
	return (0);
}

HTTPContext& Config::getHTTPBlock()
{
    return _http_block;
}

Config* Config::getInstance()
{
    return _instance;
}

const std::vector<std::string> Config::getPorts()
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