#include "Router.hpp"
#include "Config.hpp"
#include "GetHandler.hpp"
#include "PostHandler.hpp"
#include "DeleteHandler.hpp"
#include "CgiHandler.hpp"
#include "CgiResHandler.hpp"
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
#include "CgiResponse.hpp"

// Constructors
Router::Router(Server &server)
{
	_handlers["GET"] = &_getHandler;
	_handlers["POST"] = &_postHandler;
	_handlers["DELETE"] = &_deleteHandler;
	_server = &server;
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
	}
	return *this;
}

int	Router::getRequestError(const Request *request, const LocationContext &locationContext)
{
	if (request->isInvalid())
		return (400);
	if (request->isTooBigError())
		return (413);
	if (request->getUri().find("..") != std::string::npos)
		return (403);
	if (isAllowedMethod(request->getMethod(), locationContext) == false && 
		locationContext.getDirective("redirect") == "")
		return (405);
	return (0);
}

/**
 * @brief リクエストに基づきハンドラーの固有処理を呼び出す
 *
 * @param request
 * @return Response*
 */
HttpMessage *Router::routeHandler(HttpMessage &message, Socket *sock)
{
	if (CgiResponse *response = dynamic_cast<CgiResponse *>(&message))
	{
		CgiResHandler handler;
		return handler.handleMessage(*response, sock);
	}
	else if (Request *request = dynamic_cast<Request *>(&message))
	{
		ClSocket *clSock = dynamic_cast<ClSocket *>(sock);
		if (request->getHeader("connection") == "close")
			clSock->setMaxRequest(0);
		else
			clSock->decrementMaxRequest();
		request->setInfo();
		Logger::instance()->writeAccessLog(*request, *clSock);

		//　リクエストに応じたServerコンテキストを取得
		const ServerContext &serverContext = Config::instance()->getHTTPBlock()
			.getServerContext(std::to_string(clSock->getLocalPort()), request->getHeader("host"));

		const LocationContext &locationContext = serverContext.getLocationContext(request->getUri());
		if (int ErrorStatus = getRequestError(request, locationContext))
			return handleError(std::to_string(ErrorStatus), serverContext);

		const std::string &redirect = locationContext.getDirective("redirect");
		if (redirect.empty() == false)
		{
			std::map<std::string, std::string> headers;
			headers["Location"] = redirect;
			return new Response("301", headers, "");
		}
		// メソッドに対応するhandlerを呼び出し
		try
		{
			if (isConnectionCgi(*request))
			{
				// CgiSocket *cgiSock = _cgiHandler.createCgiSocket();
				_cgiHandler.init(*_server, serverContext.getLocationContext(request->getUri()));
				_cgiHandler.setClientSocket(clSock);
				return _cgiHandler.handleRequest(*request, serverContext);
			}
			else
			{
				if (_handlers.count(request->getMethod()) == 0)
					return handleError("405", serverContext);
				else
				{
					IHandler *handler = _handlers.at(request->getMethod());
					return (handler->handleRequest(*request, serverContext));
				}
			}
		}
		catch (const RequestException &e)
		{
			// ハンドラー固有の処理に失敗した場合
			return handleError(e.getStatus(), serverContext);
		}
		catch (const ServerException &e)
		{
			// サーバー固有の処理に失敗した場合
			Logger::instance()->writeErrorLog(ErrorCode::SYSTEM_CALL, e.what(), request);
			return handleError("500", serverContext);
		}
		catch (const std::out_of_range& e)
		{
			Logger::instance()->writeErrorLog(ErrorCode::NOT_METHOD, NULL, request);
			// 対応するメソッドがない場合は405を返す
			return handleError("405", serverContext);
		}
	}
	else
		return (NULL);
}

/**
 * @brief 許可されたメソッドかどうかを判定する
 *
 * @param request
 * @return true
 * @return false
 */
bool	Router::isAllowedMethod(const std::string &method, const LocationContext &locationContext)
{
	std::vector<std::string> allowedMethods = locationContext.getAllowedMethods();
	for (std::vector<std::string>::iterator it = allowedMethods.begin(); it != allowedMethods.end(); it++)
	{
		if (*it == method)
			return true;
	}
	return false;
}

/**
 * @brief CGIのリクエストか否かを判定する
 *
 * @param request
 * @return true
 * @return false
 */
bool	Router::isConnectionCgi(const Request &request)
{
	if (request.getActualUri().find(".php") != std::string::npos)
		return (true);
	return (false);
}

/**
 * @brief ステータスコードに応じたエラーページを呼び出す
 *
 * @param request リクエスト
 * @return Response レスポンス
 */

Response *Router::handleError(const std::string &status, const ServerContext &serverContext)
{
	// エラーページのパスを取得
	std::string error_path = serverContext.getErrorPage(status);

	if (error_path == "")
	{
		return (new Response(status, std::map<std::string, std::string>(), generateDefaultErrorPage()));
	}
	// 実体パスに変換
	std::string actual_path = Request::convertUriToPath(error_path, serverContext);

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
	return "<html><head><title>Error Page</title></head><body><center><h1>\
		Error Page</h1></center><hr><center>Webserv/0.0.1</center></body></html>";
}

// Not use
Router::Router()
{
	_handlers["GET"] = &_getHandler;
	_handlers["POST"] = &_postHandler;
	_handlers["DELETE"] = &_deleteHandler;
}

