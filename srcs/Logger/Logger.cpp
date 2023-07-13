#include "Logger.hpp"
#include <ctime>

Logger::Logger(const std::string& accessLogPath,
			   const std::string& errorLogPath)
	: _accessLogPath(accessLogPath), _errorLogPath(errorLogPath)
{
	_instance = this;
}

Logger::~Logger()
{	
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
		_instance = new Logger("./logs/access.log", "./logs/error.log");
	return _instance;
}

void Logger::writeAccessLog(Request request, Response response)
{
	std::ofstream logFile;
    logFile.open(_accessLogPath.c_str(), std::ios::app);
    if (logFile.is_open()) {
        // 現在の時間を取得し、それを文字列としてフォーマット
        std::time_t now = std::time(0);
        char timestamp[100];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        // ログメッセージを作成。ここではgetterメソッドを仮定しています。
        logFile << timestamp << " "
                << request.getMethod() << " "
                << request.getUri() << " "
                << response.getStatus() << " ";
        logFile.close();
    }
	else
        std::cerr << "Unable to open access log file" << std::endl;
}

void Logger::writeErrorLog(const std::string &message)
{
	std::ofstream logFile;
	logFile.open(_errorLogPath.c_str(), std::ios::app);
	if (logFile.is_open())
	{
		logFile << message << std::endl;
		logFile.close();
	}
	else
		std::cerr << "Unable to open error log file" << std::endl;
}