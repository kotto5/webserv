#include "MimeType.hpp"
#include <map>

/**
 * @brief 拡張子とMIMEタイプの対応表を作成する
 *
 * @detail RFC6838に準拠。新規のMIMEタイプはここに追加すること
 * @return 対応表
 */
std::map<std::string, std::string> createMimeType()
{
	std::map<std::string, std::string> mimeType;

	mimeType[".html"] = "text/html";
	mimeType[".css"] = "text/css";
	mimeType[".js"] = "application/javascript";
	mimeType[".jpg"] = "image/jpeg";
	mimeType[".png"] = "image/png";
	mimeType[".gif"] = "image/gif";
	mimeType[".json"] = "application/json";

	return mimeType;
};

const std::map<std::string, std::string> MimeType::MIME_TYPE = createMimeType();
