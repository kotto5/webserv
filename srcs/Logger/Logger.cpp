#include "Logger.hpp"
#include <ctime>
#include <cstring>
#include <unistd.h>

Logger::Logger(const std::string& accessLogPath, const std::string& errorLogPath)
	: _accessLogPath(accessLogPath), _errorLogPath(errorLogPath)
{
	_instance = this;

	openLogFile(_ofsAccessLog, accessLogPath);
	openLogFile(_ofsErrorLog, errorLogPath);
}

void Logger::openLogFile(std::ofstream &ofs, const std::string &logfile)
{
	if (logfile.empty())
	{
		throw std::runtime_error("Log file path is empty.");
	}
	// アクセスログファイルを開く（appendモード）
	ofs.open(logfile.c_str(), std::ios::app);
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


	// ログファイルに出力
	_ofsAccessLog << logMessage << std::endl;
	// std::cout << logMessage << std::endl;
}

/**
 * @brief エラーログを出力する
 *
 * @param msg
 * @param error_type
 */
void Logger::writeErrorLog(const ErrorCode::E_TYPE type, const std::string &message, const Request* request)
{
	std::string error_msg = "Error: ";

	// システムコールの場合はエラー番号からメッセージを取得
	if (type == ErrorCode::E_SYSCALL)
	{
		error_msg += strerror(errno);
	}
	else
	{
		error_msg += ErrorCode::getErrorMessage(type);
	}

	// タイムスタンプを取得
	std::time_t now = std::time(0);
	char timestamp[100];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

	std::string log = std::string(timestamp) + " " + error_msg + " ";
	if (request)
	{
		std::cerr << request->getMethod() + " " + request->getUri()
			+ " " + request->getProtocol() << " "
			+ std::to_string(errno) << std::endl;
	}

	log += " " + message;

	// ログファイルに出力
	_ofsErrorLog << log << std::endl;
	// std::cerr << log << std::endl;
}

Logger* Logger::_instance = NULL;

// デフォルトのログファイルパス
const std::string Logger::DEFAULT_ERROR_LOG_PATH = "./logs/error.log";
const std::string Logger::DEFAULT_ACCESS_LOG_PATH = "./logs/access.log";