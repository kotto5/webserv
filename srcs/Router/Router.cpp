#include "Router.hpp"
#include "Config.hpp"
#include "GetHandler.hpp"
#include "PostHandler.hpp"
#include "DeleteHandler.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "utils.hpp"
#include "RequestException.hpp"
#include "ServerException.hpp"
#include <iostream>
#include <string>
#include <cstdio>
#include <filesystem>
#include <sstream>

// Constructors
Router::Router()
{
	_handlers["GET"] = &_getHandler;
	_handlers["POST"] = &_postHandler;
	_handlers["DELETE"] = &_deleteHandler;
}

Router::Router(const Router &other)
{
	this->_handlers = other._handlers;
}

// Destructor
Router::~Router() {}

// Operators
Router &Router::operator=(const Router &rhs)
{
	if (this != &rhs)
	{
		this->_handlers = rhs._handlers;
	}
	return *this;
}

/**
 * @brief メソッド・URIに基づき適切なハンドラーを呼び出す
 *
 * @param request
 * @return Response*
 */
Response *Router::routeHandler(const Request &request)
{
	std::string method = request.getMethod();
	try
	{
		// メソッドに対応するhandlerを呼び出し
		IHandler *handler = _handlers.at(method);
		return handler->handleRequest(request);
	}
	catch (const RequestException &e)
	{
		return handleError(request, e.getStatus());
	}
	catch (const std::out_of_range& e)
	{
		// 対応するメソッドがない場合は405を返す
		return handleError(request, "405");
	}
}

/**
 * @brief ステータスコードに応じたエラーページを呼び出す
 *
 * @param request リクエスト
 * @return Response レスポンス
 */

Response *Router::handleError(const Request &request, const std::string &status)
{
	// エラーページのパスを取得
	std::string error_path = Config::instance()
		->getHTTPBlock()
		.getServerContext(request.getServerPort(), request.getHeader("host"))
		.getErrorPage(status);

	// 実体パスに変換
	std::string actual_path = Request::convertUriToPath(error_path, request.getServerPort(), request.getHeader("host"));

	//　エラーページが参照できない場合はデフォルトの内容を返す
	if (!pathExist(actual_path.c_str()))
	{
		std::string body = generateDefaultErrorPage("40x");
		std::map<std::string, std::string> headers;
		headers["content-type"] = "text/html";
		return new Response(status, headers, body);
	}

	// ファイルを開く
	std::ifstream htmlFile(actual_path.c_str());
	if (!htmlFile.is_open())
	{
		std::string body = generateDefaultErrorPage("50x");
		std::map<std::string, std::string> headers;
		headers["content-type"] = "text/html";
		return new Response(status, headers, body);
	}

	std::stringstream buffer;
	buffer << htmlFile.rdbuf();
	htmlFile.close();

	// レスポンスを作成して返す
	std::map<std::string, std::string> headers;
	headers["content-type"] = "text/html";
	return new Response(status, headers, buffer.str());

}

/**
 * @brief デフォルトのエラーページ内容を生成
 *
 * @param status
 * @return std::string
 */
std::string Router::generateDefaultErrorPage(const std::string &status)
{
	// 400系の場合
	if (status == "40x")
	{
		return "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>Webserv/0.0.1</center></body></html>";
	}
	return "<html><head><title>500 Internal Server Error</title></head><body><center><h1>500 Internal Server Error</h1></center><hr><center>Webserv/0.0.1</center></body></html>";
}