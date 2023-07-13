#ifndef SYSTEMERROR_HPP
#define SYSTEMERROR_HPP

#include <string>

class SystemError
{
	public:
		SystemError(const std::string& message, int code);;
		~SystemError();
		const std::string& getErrorMessage() const;
		int getErrorCode() const;
	
	private:
		std::string _errorMessage;
		int _errorCode;
};

#endif