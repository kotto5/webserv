#include "ErrorCode.hpp"
#include "iostream"

const std::string &ErrorCode::getErrorMessage(ErrorCode::e_type type)
{
	return ErrorCode::ERROR_CODE.at(type);
}

std::map<ErrorCode::e_type, std::string> createErrorCode()
{
	std::map<ErrorCode::e_type, std::string> errorCode;

	/** リクエストエラー */
	errorCode[ErrorCode::REQ_PARSE] = "Request parse failed.";
	/** サーバーエラー */
	errorCode[ErrorCode::RES_PARSE] = "Response parse failed.";
	/** POSTエラー */
	errorCode[ErrorCode::POST_INDEX_FULL] = "Error: can not create file in this name more";
	errorCode[ErrorCode::POST_FILE_OPEN] = "Error: can not open file";
	errorCode[ErrorCode::POST_NOT_EXISTS] = "Error: directory does not exist";
	errorCode[ErrorCode::POST_FILE_ACCESS] = "Error: can not access file";
	/** 設定エラー */
	errorCode[ErrorCode::CONF_DUPLICATE_DIRECTIVE] = "Config parse failed (duplicate directive).";
	errorCode[ErrorCode::CONF_NOT_ALLOWED_DIRECTIVE] = "Config parse failed (not allowed directive).";
	errorCode[ErrorCode::CONF_UNKOWN_DIRECTIVE] = "Config parse failed (unknown directive).";
	errorCode[ErrorCode::CONF_SYSTEM_ERROR] = "Config parse failed (system error).";
	return errorCode;
}

const std::map<ErrorCode::e_type, std::string> ErrorCode::ERROR_CODE = createErrorCode();
