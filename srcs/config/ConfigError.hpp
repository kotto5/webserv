#ifndef CONFIGERROR_HPP
#define CONFIGERROR_HPP

#include <string>
#include <exception>

class ConfigError : public std::exception
{
	enum ErrorType
	{
		DUPRICATE_DIRECTIVE,
		NOT_ALLOWED_DIRECTIVE,
		UNKOWN_DIRECTIVE
	};

	public:
		ConfigError(const ErrorType error_type, const std::string& error_word,
					const std::string& filepath, int line_number);
		//ConfigError(const ErrorType error_type, const std::string &error_word);
		~ConfigError() throw();
		void setErrorMessage(const ErrorType error_type, const std::string& error_word);
		const char* what() const throw();

	private:
		std::string itostr(int num);
		std::string _error_message;
		std::string _file_info;
};

#endif
