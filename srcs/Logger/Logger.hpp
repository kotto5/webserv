#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Error.hpp"
#include <fstream>
#include <string>
#include <iostream>

class Logger
{
	public:
		Logger(const std::string& accessLogPath, const std::string& errorLogPath);
		static Logger* getInstance();
		int	redirectErrorLogFile(std::string errorLogFile);
		int redirectAccessLogFile(std::string accessLogFile);
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

		// ログファイルのストリーム
		std::ofstream _ofsErrorLog;
		std::ofstream _ofsAccessLog;

		// シングルトンパターンのため外部からの変更・破棄を避ける
		~Logger();
		Logger(const Logger& other);
		Logger& operator=(const Logger& other);
};

#endif