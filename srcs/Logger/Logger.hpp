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
		Logger(const std::string& accessLogPath,
			   const std::string& errorLogPath);
		~Logger();
		static Logger* getInstance();
		const std::string& getAccessLogPath() const;
		const std::string& getErrorLogPath() const;
		void writeAccessLog(Request request, Response response);
		void writeErrorLog(const std::string &message);

	private:
		std::string _accessLogPath;
		std::string _errorLogPath;
		static Logger* _instance;
};

#endif