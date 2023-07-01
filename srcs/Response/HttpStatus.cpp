#include "HttpStatus.hpp"

/**
 * @brief HTTPステータスコードとメッセージの対応表を作成する
 *
 * @detail RFC9110に準拠
 * @return 対応表
 */
std::map<int, std::string> createHttpStatusMap()
{
	std::map<int, std::string> httpStatus;

	httpStatus[100] = "Continue";
	httpStatus[101] = "Switching Protocols";
	httpStatus[200] = "OK";
	httpStatus[201] = "Created";
	httpStatus[202] = "Accepted";
	httpStatus[203] = "Non-Authoritative Information";
	httpStatus[204] = "No Content";
	httpStatus[205] = "Reset Content";
	httpStatus[206] = "Partial Content";
	httpStatus[300] = "Multiple Choices";
	httpStatus[301] = "Moved Permanently";
	httpStatus[302] = "Found";
	httpStatus[303] = "See Other";
	httpStatus[304] = "Not Modified";
	httpStatus[305] = "Use Proxy";
	httpStatus[306] = "Unused";
	httpStatus[307] = "Temporary Redirect";
	httpStatus[308] = "Permanent Redirect";
	httpStatus[400] = "Bad Request";
	httpStatus[401] = "Unauthorized";
	httpStatus[403] = "Forbidden";
	httpStatus[404] = "Not Found";
	httpStatus[405] = "Method Not Allowed";
	httpStatus[406] = "Not Acceptable";
	httpStatus[407] = "Proxy Authentication Required";
	httpStatus[408] = "Request Timeout";
	httpStatus[409] = "Conflict";
	httpStatus[410] = "Gone";
	httpStatus[411] = "Length Required";
	httpStatus[412] = "Precondition Failed";
	httpStatus[413] = "Payload Too Large";
	httpStatus[414] = "URI Too Long";
	httpStatus[415] = "Unsupported Media Type";
	httpStatus[416] = "Range Not Satisfiable";
	httpStatus[417] = "Expectation Failed";
	httpStatus[418] = "I'm a teapot";
	httpStatus[421] = "Misdirected Request";
	httpStatus[422] = "Unprocessable Entity";
	httpStatus[423] = "Locked";
	httpStatus[424] = "Failed Dependency";
	httpStatus[425] = "Too Early";
	httpStatus[426] = "Upgrade Required";
	httpStatus[428] = "Precondition Required";
	httpStatus[429] = "Too Many Requests";
	httpStatus[431] = "Request Header Fields Too Large";
	httpStatus[451] = "Unavailable For Legal Reasons";
	httpStatus[500] = "Internal Server Error";
	httpStatus[501] = "Not Implemented";
	httpStatus[502] = "Bad Gateway";
	httpStatus[503] = "Service Unavailable";
	httpStatus[504] = "Gateway Timeout";
	httpStatus[505] = "HTTP Version Not Supported";
	httpStatus[506] = "Variant Also Negotiates";
	httpStatus[507] = "Insufficient Storage";
	httpStatus[508] = "Loop Detected";
	httpStatus[510] = "Not Extended";
	httpStatus[511] = "Network Authentication Required";

	return httpStatus;
}

const std::map<int, std::string> HttpStatus::HTTP_STATUS = createHttpStatusMap();
