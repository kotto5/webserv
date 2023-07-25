#include "Logger.hpp"
#include <ctime>
#include <cstring>
#include <unistd.h>

Logger::Logger(const std::string& accessLogPath, const std::string& errorLogPath)
	: _accessLogPath(accessLogPath), _errorLogPath(errorLogPath)
{
	_instance = this;
}

const std::string& Logger::getAccessLogPath() const
{
	return _accessLogPath;
}

const std::string& Logger::getErrorLogPath() const
{
	return _errorLogPath;
}

/**
 * @brief シングルトンパターンのため、同一のインスタンスを返す
 *
 * @return Logger*
 */
Logger* Logger::getInstance()
{
	if (_instance == NULL)
	{
		_instance = new Logger(DEFAULT_ACCESS_LOG_PATH, DEFAULT_ERROR_LOG_PATH);
	}
	return _instance;
}

/**
 * @brief アクセスログを出力する
 *
 * @param request
 * @param response
 */
void Logger::writeAccessLog(const Request& request, const Response& response)
{
	// タイムスタンプを取得
	std::time_t now = std::time(0);
	char timestamp[100];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

	// ログメッセージを作成
	std::cerr << timestamp << " "
		<< request.getMethod() << " "
		<< request.getUri() << " "
		<< request.getProtocol() << " "
		<< response.getStatus() << std::endl;
}


/**
 * @brief エラーログを出力する
 *
 * @param msg
 * @param error_code
 */
void Logger::writeErrorLog(std::string msg, Error::T_ERROR_CODE error_code)
{
	// タイムスタンプを取得
	std::time_t now = std::time(0);
	char timestamp[100];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

	std::cerr << timestamp << " ";
	// if (request)
	//     std::cerr << request->getProtocol() << " ";
	// if (response)
	//     std::cerr << response->getStatus() << " " << response->getHeader("Content-Length") << " ";
	Error::print_error(msg, error_code);
}

Logger* Logger::_instance = NULL;

const std::string Logger::DEFAULT_ERROR_LOG_PATH = "./logs/error.log";
const std::string Logger::DEFAULT_ACCESS_LOG_PATH = "./logs/access.log";