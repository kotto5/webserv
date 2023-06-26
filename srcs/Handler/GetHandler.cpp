#include "GetHandler.hpp"
#include <fstream>
#include <sstream>

// Constructors
GetHandler::GetHandler() {}

GetHandler::GetHandler(const GetHandler &other)
{
	(void)other;
}

// Destructor
GetHandler::~GetHandler() {}

// Operators
GetHandler &GetHandler::operator=(const GetHandler &rhs)
{
	if (this != &rhs)
	{
		(void)rhs;
	}
	return *this;
}

/**
 * @brief GETメソッドの処理を行う
 *
 * @param request リクエスト
 * @return Response レスポンス
 */
Response GetHandler::handleRequest(const Request &request) const
{
	// URIからファイルを開く
	std::ifstream htmlFile(request.getUri());
	if (!htmlFile.is_open())
	{
		throw std::runtime_error("Could not open file");
	}
	
	// ファイルの内容を読み込む
	std::stringstream buffer;
	buffer << htmlFile.rdbuf();
	htmlFile.close();

	// レスポンスを作成して返す
	Response res(200, std::map<std::string, std::string>(), buffer.str());
	return res;
}
