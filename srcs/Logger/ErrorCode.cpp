#include "ErrorCode.hpp"
#include "iostream"

const std::string &ErrorCode::getErrorMessage(ErrorCode::E_TYPE type)
{
	return ErrorCode::ERROR_CODE.at(type);
}

std::map<ErrorCode::E_TYPE, std::string> createErrorCode()
{
	std::map<ErrorCode::E_TYPE, std::string> errorCode;

	errorCode[ErrorCode::E_REQ_PARSE] = "Request parse failed.";
	errorCode[ErrorCode::E_RES_PARSE] = "Response parse failed.";
	errorCode[ErrorCode::E_CONF_PARSE] = "Config parse failed.";

	return errorCode;
}

const std::map<ErrorCode::E_TYPE, std::string> ErrorCode::ERROR_CODE = createErrorCode();
