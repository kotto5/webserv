#include "Logger.hpp"
#include <ctime>

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
	std::ofstream logFile;

    logFile.open(_accessLogPath.c_str(), std::ios::app);
    if (logFile.is_open())
    {
        // 現在の時間を取得し、それを文字列としてフォーマット
        std::time_t now = std::time(0);
        char timestamp[100];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        // ログメッセージを作成。ここではgetterメソッドを仮定しています。
        logFile << timestamp << " "
                << request.getMethod() << " "
                << request.getUri() << " "
                << request.getProtocol() << " "
				<< response.getStatus() << std::endl;

        logFile.close();
    }
	else
        std::cerr << "Unable to open access log file" << std::endl;
}

void Logger::writeErrorLog(const Request* request, const Response* response, const SystemError* systemError)
{
    std::ofstream logFile;
    logFile.open(_errorLogPath.c_str(), std::ios::app);
    if (logFile.is_open())
    {
        // 現在の時間を取得し、それを文字列としてフォーマット
        std::time_t now = std::time(0);
        char timestamp[100];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        // ログメッセージを作成
        logFile << timestamp << " ";

        // Request and Response information, if available
        if (request) {
            logFile << request->getProtocol() << " ";
        }
        if (response) {
            logFile << response->getStatus() << " " << response->getHeader("Content-Length") << " ";
        }

        // SystemError information
        if (systemError) {
            logFile << systemError->getErrorMessage() << " " << systemError->getErrorCode();
        }

        logFile << std::endl;
        logFile.close();
    }
    else
        std::cerr << "Unable to open error log file" << std::endl;
}

Logger* Logger::_instance = NULL;

const std::string Logger::DEFAULT_ERROR_LOG_PATH = "./logs/error.log";
const std::string Logger::DEFAULT_ACCESS_LOG_PATH = "./logs/access.log";


// シングルトンパターンのため外部からの変更・破棄を避ける
Logger::~Logger()
{
}

Logger::Logger(const Logger& other)
{
	*this = other;
}

Logger& Logger::operator=(const Logger& other)
{
	if (this != &other)
	{
		_accessLogPath = other._accessLogPath;
		_errorLogPath = other._errorLogPath;
	}
	return *this;
}