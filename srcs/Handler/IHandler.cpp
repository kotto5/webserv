#include "IHandler.hpp"
#include "utils.hpp"
#include <sstream>
#include <fstream>

/**
 * @brief デフォルトのエラーページ内容を生成
 *
 * @param status
 * @return std::string
 */
std::string generateDefaultErrorPage()
{
	return "<html><head><title>Error Page</title></head><body><center><h1>\
		Error Page</h1></center><hr><center>Webserv/0.0.1</center></body></html>";
}

/**
 * @brief ステータスコードに応じたエラーページを呼び出す
 *
 * @param request リクエスト
 * @return Response レスポンス
 */
HttpMessage	*IHandler::handleError(const std::string &status, const ServerContext &serverContext)
{
	// エラーページのパスを取得
	std::string error_path = serverContext.getErrorPage(status);

	if (error_path == "")
	{
		return (new(std::nothrow) Response(status, std::map<std::string, std::string>(), generateDefaultErrorPage()));
	}
	// 実体パスに変換
	std::string actual_path = Request::convertUriToPath(error_path, serverContext);

	//　エラーページが参照できない場合はデフォルトの内容を返す
	if (!pathExist(actual_path.c_str()))
	{
		std::string body = generateDefaultErrorPage();
		std::map<std::string, std::string> headers;
		headers["content-type"] = "text/html";
		return (new(std::nothrow) Response(status, headers, body));
	}

	// ファイルを開く
	std::ifstream htmlFile(actual_path.c_str());
	if (!htmlFile.is_open())
	{
		std::string body = generateDefaultErrorPage();
		std::map<std::string, std::string> headers;
		headers["content-type"] = "text/html";
		return (new(std::nothrow) Response(status, headers, body));
	}

	std::stringstream buffer;
	buffer << htmlFile.rdbuf();
	htmlFile.close();

	// レスポンスを作成して返す
	std::map<std::string, std::string> headers;
	headers["content-type"] = "text/html";
	return (new(std::nothrow) Response(status, headers, buffer.str()));
}

HttpMessage	*IHandler::handleError(const std::string &status, int port)
{
	return handleError(status, Request::getServerContext(port));
}