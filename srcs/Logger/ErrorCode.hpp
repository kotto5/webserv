#ifndef ERROR_CODE_HPP
#define ERROR_CODE_HPP

#include <map>
#include <string>

class ErrorCode
{
public:
	typedef enum {
		SYSTEM_CALL, // システムコールの失敗
		REQ_PARSE, // リクエストの解析に失敗
		RES_PARSE, // レスポンスの解析に失敗
		DELETE_FILE_NOT_EXIST, // 削除しようとしたファイルが存在しない
		DELETE_FILE_NO_PERMISSION, // 削除しようとしたファイルに対するパーミッションがない
		CONF_DUPLICATE_DIRECTIVE, // 設定ファイル：重複したディレクティブ
		CONF_NOT_ALLOWED_DIRECTIVE, // 設定ファイル：許可されていないディレクティブ
		CONF_UNKOWN_DIRECTIVE, // 設定ファイル：未知のディレクティブ
		CONF_SYSTEM_ERROR // 設定ファイル
	}	e_type;

	static const std::map<e_type, std::string> ERROR_CODE;
	static const std::string &getErrorMessage(e_type type);
};

#endif
