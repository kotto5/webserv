#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Request.hpp"
#include "Response.hpp"
#include <fstream>
#include <string>
#include <iostream>

class Logger
{
	public:
		class Error
		{
			public:
				typedef enum E_ERROR_CODE{
					E_SYSCALL,
					E_REQ_PARSE,
					E_RES_PARSE,
				}   T_ERROR_CODE;

				static const std::map<Error::T_ERROR_CODE, std::string> createErrorMap();
				static void print_error(std::string msg, T_ERROR_CODE error_code);
				static const std::map<T_ERROR_CODE, std::string> error_msg;

			private:
				Error();
				~Error();
		};
		Logger(const std::string& accessLogPath,
			   const std::string& errorLogPath);
		~Logger();
		static Logger* getInstance();
		const std::string& getAccessLogPath() const;
		const std::string& getErrorLogPath() const;
		void writeAccessLog(const Request& request, const Response& response);
		void writeErrorLog(std::string msg, Error::T_ERROR_CODE error_code);

	private:
		static const std::string DEFAULT_ACCESS_LOG_PATH;
		static const std::string DEFAULT_ERROR_LOG_PATH;
		std::string _accessLogPath;
		std::string _errorLogPath;
		static Logger* _instance;
};

#endif