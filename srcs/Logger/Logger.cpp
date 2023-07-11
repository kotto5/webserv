#include "Logger.hpp"

Logger::Logger()
{}

Logger::~Logger()
{}

Logger* Logger::getInstance()
{
    if (_instance == NULL)
		_instance = new Logger();
	return _instance;
}

void Logger::setAccessLogPath(const std::string &path)
{
	_accessLogPath = path;
}

void Logger::setErrorLogPath(const std::string &path)
{
	_errorLogPath = path;
}

void Logger::writeAccessLog(const std::string &message)
{
	std::ofstream logFile;
	logFile.open(_accessLogPath.c_str(), std::ios::app);
	if (logFile.is_open())
	{
		logFile << message << std::endl;
		logFile.close();
	}
	else
		std::cerr << "Unable to open access log file" << std::endl;
}

void Logger::writeErrorLog(const std::string &message)
{
	std::ofstream logFile;
	logFile.open(_errorLogPath.c_str(), std::ios::app);
	if (logFile.is_open())
	{
		logFile << message << std::endl;
		logFile.close();
	}
	else
		std::cerr << "Unable to open error log file" << std::endl;
}