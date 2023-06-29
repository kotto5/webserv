#ifndef HTTPCONTEXT_HPP
#define HTTPCONTEXT_HPP

#include <string>

class HTTPContext
{
	public:
		HTTPContext();
		~HTTPContext();
		void setAccessLogFile(const std::string& accessLogFile);
		void setErrorLogFile(const std::string& errorLogFile);
		const std::string& getAccessLogFile() const;
		const std::string& getErrorLogFile() const;

	private:
		std::string _accessLogFile;
		std::string _errorLogFile;
};

#endif