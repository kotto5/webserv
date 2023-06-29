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
