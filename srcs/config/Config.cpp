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

Config::~Config()
{
    close(newfd);
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
