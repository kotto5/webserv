#include "Logger.hpp"
#include <ctime>
#include <cstring>
#include <unistd.h>

Logger::Logger(const std::string& accessLogPath, const std::string& errorLogPath)
	: _accessLogPath(accessLogPath), _errorLogPath(errorLogPath)
{
	_instance = this;

	this->redirectAccessLogFile(accessLogPath);
	this->redirectErrorLogFile(errorLogPath);
}

int Logger::redirectAccessLogFile(std::string  accessLogFile)
{
	if (accessLogFile.empty())
	{
		return (0);
	}
	// アクセスログファイルを開く（appendモード）
	_ofsAccessLog.open(accessLogFile.c_str(), std::ios::app);

	return (0);
}

int Logger::redirectErrorLogFile(std::string errorLogFile)
{
	if (errorLogFile.empty())
	{
		return (0);
	}
	// エラーログファイルを開く（appendモード）
	_ofsErrorLog.open(errorLogFile.c_str(), std::ios::app);

	// 標準エラー出力をリダイレクトする
	std::cerr.rdbuf(_ofsErrorLog.rdbuf());

	return (0);

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
	std::string logMessage = std::string(timestamp) + " " + request.getMethod() + " "
		+ request.getUri() + " " + request.getProtocol() + " "
		+ std::to_string(response.getStatus());

	// 標準出力とログファイルに出力
	std::cout << logMessage;
	_ofsAccessLog << logMessage;
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

	std::string logMessage = std::string(timestamp) + " ";
	// if (request)
	// 	std::cerr << request->getProtocol() << " ";
	// if (response)
	// 	std::cerr << response->getStatus() << " " << response->getHeader("Content-Length") << " ";

	// 標準エラー出力とログファイルに出力
	std::cout << logMessage;
	_ofsErrorLog << logMessage;

	Error::print_error(msg, error_code);

}

Logger* Logger::_instance = NULL;

const std::string Logger::DEFAULT_ERROR_LOG_PATH = "./logs/error.log";
const std::string Logger::DEFAULT_ACCESS_LOG_PATH = "./logs/access.log";