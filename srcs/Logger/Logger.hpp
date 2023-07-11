#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>
#include <iostream>

class Logger
{
	public:
		static Logger* getInstance();
		void setAccessLogPath(const std::string &path);
		void setErrorLogPath(const std::string &path);
		void writeAccessLog(const std::string &message);
		void writeErrorLog(const std::string &message);

	private:
		// コンストラクタとデストラクタをプライベートにしてインスタンス化を制限
		Logger();
		~Logger();
		std::string _accessLogPath;
		std::string _errorLogPath;
		static Logger* _instance;
};

#endif