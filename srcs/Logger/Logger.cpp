#include "Logger.hpp"
#include <ctime>
#include <cstring>
#include <unistd.h>
#include "ServerException.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

void Logger::initialize(const std::string& accessLogPath, const std::string& errorLogPath)
{
	if (_instance != NULL)
	{
		throw ServerException("Logger instance already exists.");
	}
	_instance = new Logger();
	// ファイルストリームを保持する
	_instance->openLogFile(_instance->_ofsAccessLog, accessLogPath);
	_instance->openLogFile(_instance->_ofsErrorLog, errorLogPath);
}

void Logger::release()
{
	if (_instance)
	{
		_instance->_ofsAccessLog.close();
		_instance->_ofsErrorLog.close();
		delete _instance;
		_instance = NULL;
	}
}

void Logger::openLogFile(std::ofstream &ofs, const std::string &logfile)
{
	if (logfile.empty())
	{
		throw ServerException("Log file path is empty.");
	}
	// アクセスログファイルを開く（appendモード）
	ofs.open(logfile.c_str(), std::ios::app);
}

/**
 * @brief シングルトンパターンのため、同一のインスタンスを返す
 *
 * @return Logger*
 */
Logger* Logger::instance()
{
	if (_instance == NULL)
	{
		throw ServerException("Logger is not initialized.");
	}
	return _instance;
}

/**
 * @brief アクセスログを出力する
 *
 * @param request
 * @param response
 */
void Logger::writeAccessLog(const Request& request, const ClSocket& clSock)
{
	// タイムスタンプを取得
	std::time_t now = std::time(0);
	char timestamp[100];
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

	const sockaddr_in &clientAddr = clSock.getRemoteAddr();
	_accessData[&clientAddr] = new std::stringstream();
	*_accessData[&clientAddr] << std::string(timestamp) << " "
		<< inet_ntoa(clientAddr.sin_addr) << " " 
		<< ntohs(clientAddr.sin_port) << " "	
		<< "Method["	<< request.getMethod()	<< "] "
		<< "Uri["		<< request.getUri()		<< "] "
		<< "Protocol["	<< request.getProtocol() << "] ";
}

void Logger::writeAccessLog(const Response& response, const ClSocket& clSock)
{
	const sockaddr_in &clientAddr = clSock.getRemoteAddr();
	_ofsAccessLog << _accessData[&clientAddr]->str() << response.getStatus() << std::endl;
	delete (_accessData[&clientAddr]);
	_accessData.erase(&clientAddr);
}

/**
 * @brief エラーログを出力する
 *
 * @param msg
 * @param error_type
 */
void Logger::writeErrorLog(const ErrorCode::e_type type, const std::string &message, const Request* request)
{
	std::string error_msg = "Error: ";
	std::string request_info = "";

	// システムコールの場合はエラー番号からメッセージを取得
	if (type == ErrorCode::SYSTEM_CALL)
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

	// リクエスト情報を取得
	if (request)
	{
		request_info = request->getMethod() + " " + request->getUri()
			+ " " + request->getProtocol() + " ";
	}

	// ログファイルに出力
	_ofsErrorLog << timestamp << " " << error_msg << " " << message << " " << request_info << std::endl;
	/** 標準エラーに出力する場合は以下をコメントアウト */
	// std::cerr << log << std::endl;
}

const std::ofstream &Logger::getAccessLogStream() const
{
	return _ofsAccessLog;
}

const std::ofstream &Logger::getErrorLogStream() const
{
	return _ofsErrorLog;
}

Logger::Logger(){};
Logger::~Logger(){};

Logger* Logger::_instance = NULL;

// デフォルトのログファイルパス
const std::string Logger::DEFAULT_ERROR_LOG_PATH = "./logs/error.log";
const std::string Logger::DEFAULT_ACCESS_LOG_PATH = "./logs/access.log";