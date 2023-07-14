#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "SystemError.hpp"
#include <fstream>
#include <string>
#include <iostream>

class Logger
{
	#define DEFAULT_ACCESS_LOG_PATH "./logs/access.log"
	#define DEFAULT_ERROR_LOG_PATH "./logs/error.log"

	public:
		Logger(const std::string& accessLogPath,
			   const std::string& errorLogPath);
		~Logger();
		static Logger* getInstance();
		const std::string& getAccessLogPath() const;
		const std::string& getErrorLogPath() const;
		void writeAccessLog(Request request, Response response);
		void writeErrorLog(Request* request = NULL, Response* response = NULL, SystemError* systemError = NULL);

	private:
		std::string _accessLogPath;
		std::string _errorLogPath;
		static Logger* _instance;
};

#endif