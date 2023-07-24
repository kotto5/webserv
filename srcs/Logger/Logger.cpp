#include "Logger.hpp"
#include <ctime>
#include <cstring>
#include <unistd.h>

#define ACCESS_LOG_FD 3

Logger::Logger(const std::string& accessLogPath,
			   const std::string& errorLogPath)
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

Logger* Logger::getInstance()
{
    if (_instance == NULL)
		_instance = new Logger(DEFAULT_ACCESS_LOG_PATH,
                                DEFAULT_ERROR_LOG_PATH);
	return _instance;
}

void Logger::writeAccessLog(const Request& request, const Response& response)
{
    std::time_t now = std::time(0);
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        // ログメッセージを作成。ここではgetterメソッドを仮定しています。
        // logFile << timestamp << " "
        //         << request.getMethod() << " "
        //         << request.getUri() << " "
        //         << request.getProtocol() << " "
		// 		<< response.getStatus() << std::endl;
    write(ACCESS_LOG_FD, timestamp, strlen(timestamp));
    write(ACCESS_LOG_FD, " ", 1);
    write(ACCESS_LOG_FD, request.getMethod().c_str(), request.getMethod().length());
    write(ACCESS_LOG_FD, " ", 1);
    write(ACCESS_LOG_FD, request.getUri().c_str(), request.getUri().length());
    write(ACCESS_LOG_FD, " ", 1);
    write(ACCESS_LOG_FD, request.getProtocol().c_str(), request.getProtocol().length());
    write(ACCESS_LOG_FD, " ", 1);
    write(ACCESS_LOG_FD, response.getStatus().c_str(), response.getStatus().length());
    write(ACCESS_LOG_FD, "\n", 1);
}

void Logger::writeErrorLog(std::string msg, Error::T_ERROR_CODE error_code)
{
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

//Error class method
void  Logger::Error::print_error(std::string msg, Logger::Error::T_ERROR_CODE error_code) {
    if (error_code == Logger::Error::E_SYSCALL)
        perror(msg.c_str());
    else
        std::cerr << msg << ": " << Logger::Error::error_msg.at(error_code) << std::endl;
}

const std::map<Logger::Error::T_ERROR_CODE, std::string> Logger::Error::createErrorMap() {
    std::map<Logger::Error::T_ERROR_CODE, std::string> error_msg;

    error_msg[Logger::Error::E_REQ_PARSE] = "Request parse error";
    error_msg[Logger::Error::E_RES_PARSE] = "Response parse error";

    return (error_msg);
}

const std::map<Logger::Error::T_ERROR_CODE, std::string> Logger::Error::error_msg = Logger::Error::createErrorMap();

Logger::Error::Error()
{
}

Logger::Error::~Error()
{
}