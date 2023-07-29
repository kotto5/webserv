#ifndef ERROR_CODE_HPP
#define ERROR_CODE_HPP

#include <map>
#include <string>

class ErrorCode
{
public:
	typedef enum {
		E_SYSCALL, // システムコールの失敗
		E_REQ_PARSE, // リクエストの解析に失敗
		E_RES_PARSE, // レスポンスの解析に失敗
		E_CONF_PARSE, // 設定ファイルの解析に失敗
	}	E_TYPE;

	static const std::map<E_TYPE, std::string> ERROR_CODE;
	static const std::string &getErrorMessage(E_TYPE type);
};

#endif
