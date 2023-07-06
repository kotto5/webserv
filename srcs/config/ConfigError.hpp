#ifndef CONFIGERROR_HPP
#define CONFIGERROR_HPP

#include <exception>
#include <string>
#include "HTTPContext.hpp"

class ConfigError : public std::exception
{
	enum ErrorType
	{
		DUPRICATE_DIRECTIVE,
		NOT_ALLOWED_DIRECTIVE,
		UNKOWN_DIRECTIVE
	};

	public:
		ConfigError(const ErrorType error_type, const std::string &error_word,
					const std::string &filepath, int line_pos);
		//ConfigError(const ErrorType error_type, const std::string &error_word);
		virtual ~ConfigError();
		virtual const char *what() const;
};

#endif
