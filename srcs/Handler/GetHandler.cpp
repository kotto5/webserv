#include "GetHandler.hpp"
#include "Autoindex.hpp"
#include "Config.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "Response.hpp"
#include "HttpMessage.hpp"
#include "RequestException.hpp"
#include "Logger.hpp"

// Constructors
GetHandler::GetHandler()
{
	// 規定値を200に設定
	this->_status = "200";
}

GetHandler::GetHandler(const GetHandler &other)
{
	*this = other;
}

// Destructor
GetHandler::~GetHandler() {}

// Operators
GetHandler &GetHandler::operator=(const GetHandler &rhs)
{
	if (this != &rhs)
	{
		this->_status = rhs._status;
	}
	return *this;
}

/**
 * @brief GETメソッドの処理を行う
 *
 * @param request リクエスト
 * @return Response レスポンス
 */

HttpMessage *GetHandler::handleRequest(const Request &request, const ServerContext &serverContext)
{
	// 実体パスを取得
	std::string actualPath = request.getActualUri();
	printf("test1\n");

	// 設定が有効、かつディレクトリの場合はAutoindexを作成
	if (isDirectory(actualPath.c_str()) && enableAutoindex(request, serverContext))
	{
		Autoindex index = Autoindex(request);
		std::string body = index.generateAutoindex();

		std::map<std::string, std::string> headers;
		headers["Content-Type"] = "text/html";
		return new Response(_status, headers, body);
	}
	printf("test1\n");

	// URIからファイルを開く
	std::ifstream htmlFile(request.getActualUri());
	if (!htmlFile.is_open())
	{
		// ファイルが開けなかった場合は404を返す
		Logger::instance()->writeErrorLog(ErrorCode::GET_FILE_NOT_EXIST, "File not found");
		throw RequestException("404");
	}

	printf("test1\n");
	// ファイルの内容を読み込む
	std::stringstream buffer;
	buffer << htmlFile.rdbuf();
	htmlFile.close();

	printf("test1\n");
	// レスポンスを作成して返す
	std::map<std::string, std::string> headers;
	HttpMessage::setHeader(headers, "content-type", Response::getMimeType(request.getActualUri()));
	return (new Response(this->_status, headers, buffer.str()));
}

/**
 * @brief Autoindexが有効かどうか
 *
 * @return true
 * @return false
 */
bool GetHandler::enableAutoindex(const Request &request, const ServerContext &serverContext)
{
	try
	{
		return serverContext.getLocationContext(request.getUri()).getDirective("autoindex") == "on";
	}
	catch (const std::runtime_error &e)
	{
		return false;
	}
}