#include "Router.hpp"
#include "Config.hpp"
#include "GetHandler.hpp"
#include "PostHandler.hpp"
#include "DeleteHandler.hpp"
#include "CgiHandler.hpp"
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
Router::Router(Server &server)
{
	_handlers["GET"] = &_getHandler;
	_handlers["POST"] = &_postHandler;
	_handlers["DELETE"] = &_deleteHandler;
	_server = &server;
	_serverContext = NULL;
}

Router::Router(const Router &other)
{
	*this = other;
}

// Destructor
Router::~Router() {}

// Operators
Router &Router::operator=(const Router &rhs)
{
	if (this != &rhs)
	{
		this->_handlers = rhs._handlers;
		this->_server = rhs._server;
		this->_serverContext = rhs._serverContext;
	}
	return *this;
}

bool	Router::isRedirect(const Request &request) const
{
	std::string dest = _serverContext->getLocationContext(request.getUri()).getDirective("redirect");
	if (dest.empty())
		return (false);
	return (true);
}

bool	Router::isAllowedMethod(const Request& request) const
{
	std::vector<std::string> allowedMethods = _serverContext->getLocationContext(request.getUri()).getAllowedMethods();
	for (std::vector<std::string>::iterator it = allowedMethods.begin(); it != allowedMethods.end(); it++)
	{
		if (*it == request.getMethod())
			return true;
	}
	return false;
}

/**
 * @brief リクエストに基づき適切なハンドラーを呼び出す
 *
 * @param request
 * @return Response*
 */
Response *Router::routeHandler(const Request &request, Socket *sock)
{
	//　リクエストに応じたServerコンテキストを取得
	_serverContext = &Config::instance()->getHTTPBlock().getServerContext(request.getServerPort(), request.getHeader("host"));

	// リクエスト時点でのエラーハンドリング
	if (request.isBadRequest())
	{
		return (new Response("400"));
	}
	else if (request.isTooBigError())
	{
		return (new Response("401"));
	}
	else if (request.getUri().find("..") != std::string::npos)
	{
		return (new Response("403"));
	}
	else if (isAllowedMethod(request) == false)
	{
		return new Response("405");
	}

	// リダイレクトの場合
	if (isRedirect(request))
	{
		std::map<std::string, std::string> headers;
		headers["Location"] = _serverContext->getLocationContext(request.getUri()).getDirective("redirect");
		return (new Response("301", headers, ""));
	}

	// CGIの場合
	if (requestWantsCgi(request))
	{
		CgiHandler handler(_server);
		handler.setClientSocket(sock);
		handler.handleRequest(request);
		return NULL;
	}

	// メソッドに対応するhandlerを呼び出し
	try
	{
		IHandler *handler = _handlers.at(request.getMethod());
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
	std::string error_path = _serverContext->getErrorPage(status);

	if (error_path == "")
	{
		return (new Response(status, std::map<std::string, std::string>(), generateDefaultErrorPage()));
	}
	// 実体パスに変換
	std::string actual_path = Request::convertUriToPath(error_path, request.getServerPort(), request.getHeader("host"));

	//　エラーページが参照できない場合はデフォルトの内容を返す
	if (!pathExist(actual_path.c_str()))
	{
		std::string body = generateDefaultErrorPage();
		std::map<std::string, std::string> headers;
		headers["content-type"] = "text/html";
		return new Response(status, headers, body);
	}

	// ファイルを開く
	std::ifstream htmlFile(actual_path.c_str());
	if (!htmlFile.is_open())
	{
		std::string body = generateDefaultErrorPage();
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
std::string Router::generateDefaultErrorPage()
{
	return "<html><head><title>Error Page</title></head><body><center><h1>Error Page</h1></center><hr><center>Webserv/0.0.1</center></body></html>";
}

/**
 * @brief CGIのリクエストか否かを判定する
 *
 * @param request
 * @return true
 * @return false
 */
bool	Router::requestWantsCgi(const Request &request)
{
	if (request.getActualUri().find(".php") != std::string::npos)
		return (true);
	return (false);
}


// Not use
Router::Router()
{
	_handlers["GET"] = &_getHandler;
	_handlers["POST"] = &_postHandler;
	_handlers["DELETE"] = &_deleteHandler;
	_serverContext = NULL;
}

