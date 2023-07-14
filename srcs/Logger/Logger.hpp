#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "../Error/SystemError.hpp"
#include <fstream>
#include <string>
#include <iostream>

class Logger
{
	public:
		Logger(const std::string& accessLogPath,
			   const std::string& errorLogPath);
		~Logger();
		static Logger* getInstance();
		const std::string& getAccessLogPath() const;
		const std::string& getErrorLogPath() const;
		void writeAccessLog(const Request& request, const Response& response);
		void writeErrorLog(const Request* request = NULL, const Response* response = NULL, const SystemError* systemError = NULL);

	private:
		static const std::string DEFAULT_ACCESS_LOG_PATH;
		static const std::string DEFAULT_ERROR_LOG_PATH;
		std::string _accessLogPath;
		std::string _errorLogPath;
		static Logger* _instance;
};

#endif