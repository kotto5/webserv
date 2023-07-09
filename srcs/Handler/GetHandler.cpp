#include "GetHandler.hpp"
#include <fstream>
#include <sstream>

// Constructors
GetHandler::GetHandler()
{
	// 規定値を200に設定
	this->_status = 200;
}

GetHandler::GetHandler(const GetHandler &other)
{
	this->_status = other._status;
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
Response GetHandler::handleRequest(const Request &request)
{
	// URIからファイルを開く
	std::ifstream htmlFile(request.getUri());
	if (!htmlFile.is_open())
	{
		// ファイルが開けなかった場合は404を返す
		this->_status = 404;
	}

	// ファイルの内容を読み込む
	std::stringstream buffer;
	buffer << htmlFile.rdbuf();
	htmlFile.close();

	// レスポンスを作成して返す
	std::map<std::string, std::string> headers;
	headers["Content-Type"] = Response::getMimeType(request.getUri());
	Response res(_status, headers, buffer.str());

	return res;
}
