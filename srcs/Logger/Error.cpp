#include "Error.hpp"
#include "iostream"

void   Error::print_error(std::string msg, Error::T_ERROR_CODE error_code)
{
	if (error_code == Error::E_SYSCALL)
	{
		perror(msg.c_str());
	}
	else
	{
		std::cerr << msg << ": " << Error::error_msg.at(error_code) << std::endl;
	}
}

/**
 * @brief エラーメッセージのマップを作成
 *
 * @return const std::map<Error::T_ERROR_CODE, std::string>
 */
const std::map<Error::T_ERROR_CODE, std::string> Error::createErrorMap()
{
	std::map<Error::T_ERROR_CODE, std::string> error_msg;

	error_msg[Error::E_REQ_PARSE] = "Request parse error"; // リクエストの解析に失敗
	error_msg[Error::E_RES_PARSE] = "Response parse error"; // レスポンスの解析に失敗
	error_msg[Error::E_CONF_PARSE] = "Config parse error"; // 設定ファイルの解析に失敗

	return (error_msg);
}

const std::map<Error::T_ERROR_CODE, std::string> Error::error_msg = Error::createErrorMap();

Error::Error() {}

Error::~Error() {}

