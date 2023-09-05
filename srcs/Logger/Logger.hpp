#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "ErrorCode.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include "Socket.hpp"
#include <sstream>

class Logger
{
	public:
		void static initialize(
			const std::string& accessLogPath = DEFAULT_ACCESS_LOG_PATH,
			const std::string& errorLogPath = DEFAULT_ERROR_LOG_PATH
		);
		void static release();
		static Logger* instance();
		void openLogFile(std::ofstream &ofs, const std::string &logFile);
		void writeErrorLog(const ErrorCode::e_type type, const std::string &message = "", const Request* request = NULL);
		const std::ofstream &getAccessLogStream() const;
		const std::ofstream &getErrorLogStream() const;
		void 				writeAccessLog(const Request& request, const ClSocket& clSock);
		void 				writeAccessLog(const Response& response, const ClSocket& clSock);

	private:
		static const std::string DEFAULT_ACCESS_LOG_PATH;
		static const std::string DEFAULT_ERROR_LOG_PATH;
		static Logger* _instance;

		// ログファイルのストリーム
		std::ofstream _ofsAccessLog;
		std::ofstream _ofsErrorLog;
		std::map<const sockaddr_in*, std::ostringstream> _accessData;

		// シングルトンパターンのため外部からの変更・破棄を避ける
		Logger();
		~Logger();
		Logger(const Logger& other);
		Logger& operator=(const Logger& other);
};

#endif