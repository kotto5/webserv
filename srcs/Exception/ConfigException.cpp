#include "ConfigException.hpp"
#include <errno.h>
#include <cstring>

ConfigException::ConfigException(
	const ErrorCode::e_type error_type,
	const std::string& error_word,
	const std::string& filepath,
	int line_number
	): ServerException("Config Error: "), _file_info(" in " + filepath + ": " + itostr(line_number))
{
	setErrorMessage(error_type, error_word);
}

ConfigException::ConfigException(
	const ErrorCode::e_type error_type,
	const std::string& error_word
	): ServerException("Config Error: ")
{
	setErrorMessage(error_type, error_word);
}

ConfigException::~ConfigException() throw()
{
}

void ConfigException::setErrorMessage(const ErrorCode::e_type error_type, const std::string& error_word)
{
	switch (error_type)
	{
		case ErrorCode::CONF_DUPLICATE_DIRECTIVE:
			_error_message += "duplicate \"" + error_word + "\" directive" + _file_info;
			break;
		case ErrorCode::CONF_NOT_ALLOWED_DIRECTIVE:
			_error_message += "\"" + error_word + "\" directive is not allowed here" + _file_info;
			break;
		case ErrorCode::CONF_UNKOWN_DIRECTIVE:
			_error_message += "unknown directive \"" + error_word + "\"" + _file_info;
			break;
		case ErrorCode::CONF_SYSTEM_ERROR:
			_error_message += "system call error \"" + error_word + ": " + strerror(errno) + "\"" + _file_info;
			break;
		default:
			break;
	}
}

const char* ConfigException::what() const throw()
{
	return _error_message.c_str();
}

std::string ConfigException::itostr(int num)
{
	std::string str;

	if (num == 0)
		return "0";
	while (num)
	{
		str = (char)(num % 10 + '0') + str;
		num /= 10;
	}
	return str;
}
