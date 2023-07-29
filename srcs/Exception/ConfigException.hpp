#ifndef CONFIG_EXCEPTION_HPP
#define CONFIG_EXCEPTION_HPP

#include <string>
#include "ServerException.hpp"
#include "ErrorCode.hpp"


class ConfigException : public ServerException
{
	public:
		ConfigException(const ErrorCode::e_type error_type, const std::string& error_word);
		ConfigException(const ErrorCode::e_type error_type, const std::string& error_word,
					const std::string& filepath, int line_number);
		~ConfigException() throw();
		void setErrorMessage(const ErrorCode::e_type error_type, const std::string& error_word);
		const char* what() const throw();

	private:
		ConfigException();
		std::string itostr(int num);
		std::string _error_message;
		std::string _file_info;
};

#endif
