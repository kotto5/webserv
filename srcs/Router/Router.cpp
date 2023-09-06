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
			.getServerContext(to_string(clSock->getLocalPort()), request->getHeader("host"));

		const LocationContext &locationContext = serverContext.getLocationContext(request->getUri());
		if (int ErrorStatus = getRequestError(request, locationContext))
			return IHandler::handleError(to_string(ErrorStatus), serverContext);

		const std::string &redirect = locationContext.getDirective("redirect");
		if (redirect.empty() == false)
		{
			std::map<std::string, std::string> headers;
			headers["Location"] = redirect;
			return new Response("301", headers, "");
		}
		// メソッドに対応するhandlerを呼び出し
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
				return IHandler::handleError("405", serverContext);
			else
			{
				IHandler *handler = _handlers.at(request->getMethod());
				return (handler->handleRequest(*request, serverContext));
			}
		}
	}
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

