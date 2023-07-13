#include "SystemError.hpp"

SystemError::SystemError(const std::string& message, int code)
	: _errorMessage(message), _errorCode(code)
{
}

SystemError::~SystemError()
{
}

const std::string& SystemError::getErrorMessage() const
{
	return _errorMessage;
}

int SystemError::getErrorCode() const
{
	return _errorCode;
}

