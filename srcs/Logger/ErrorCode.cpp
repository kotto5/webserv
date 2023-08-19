#include "ErrorCode.hpp"
#include "iostream"

const std::string &ErrorCode::getErrorMessage(ErrorCode::e_type type)
{
	return ErrorCode::ERROR_CODE.at(type);
}

std::map<ErrorCode::e_type, std::string> createErrorCode()
{
	std::map<ErrorCode::e_type, std::string> errorCode;

	/** サーバーエラー */
	errorCode[ErrorCode::RECV_TIMEOUT] = "Receive time out.";
	errorCode[ErrorCode::SEND_TIMEOUT] = "Send time out.";
	errorCode[ErrorCode::REQ_PARSE] = "Request parse failed.";
	errorCode[ErrorCode::RES_PARSE] = "Response parse failed.";
	errorCode[ErrorCode::NOT_METHOD] = "Not allowed method.";
	/** GETエラー */
	errorCode[ErrorCode::GET_FILE_NOT_EXIST] = "Get file not exists.";
	/** POSTエラー */
	errorCode[ErrorCode::POST_INDEX_FULL] = "Error: can not create file in this name more";
	errorCode[ErrorCode::POST_FILE_OPEN] = "Error: can not open file";
	errorCode[ErrorCode::POST_NOT_EXISTS] = "Error: directory does not exist";
	errorCode[ErrorCode::POST_FILE_ACCESS] = "Error: can not access file";
	// DELETEエラー
	errorCode[ErrorCode::DELETE_FILE_NOT_EXIST] = "Delete file not exists.";
	errorCode[ErrorCode::DELETE_FILE_NO_PERMISSION] = "Delete file has no permission.";
	// CGIエラー

	// Autoindexエラー
	errorCode[ErrorCode::AUTO_FILE_NOT_OPEN] = "Autoindex file open failed.";
	/** 設定エラー */
	errorCode[ErrorCode::CONF_DUPLICATE_DIRECTIVE] = "Config parse failed (duplicate directive).";
	errorCode[ErrorCode::CONF_NOT_ALLOWED_DIRECTIVE] = "Config parse failed (not allowed directive).";
	errorCode[ErrorCode::CONF_UNKOWN_DIRECTIVE] = "Config parse failed (unknown directive).";
	errorCode[ErrorCode::CONF_SYSTEM_ERROR] = "Config parse failed (system error).";
	return errorCode;
}

const std::map<ErrorCode::e_type, std::string> ErrorCode::ERROR_CODE = createErrorCode();
