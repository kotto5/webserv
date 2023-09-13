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
	if (request->isInvalid() || !request->isCompleted())
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

HttpMessage	*Router::makeSendMessage(HttpMessage &message, Socket *sock)
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

		const LocationContext &locationContext = request->getLocationContext();
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
		if (locationContext.getDirective("cgi_pass") != "")
			return _cgiHandler.handleRequest(*request, serverContext);
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
	else
		return (NULL);
}

/**
 * @brief リクエストに基づきハンドラーの固有処理を呼び出す
 *
 * @param request
 * @return Response*
 */
HttpMessage *Router::routeHandler(HttpMessage &message, Socket *sock)
{
	HttpMessage *sendMessage = NULL;
	sendMessage = makeSendMessage(message, sock);
	if (Response *response = dynamic_cast<Response *>(sendMessage))
	{
		ClSocket *clSock;
		clSock = dynamic_cast<ClSocket *>(sock);
		if (clSock == NULL)
		{
			CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock);
			clSock = cgiSock->getClSocket();
		}
		addKeepAliveHeader(response, clSock);
		Logger::instance()->writeAccessLog(*response, *clSock);
	}
	return sendMessage;
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

void	Router::addKeepAliveHeader(Response *response, ClSocket *clientSock)
{
	if (clientSock->getMaxRequest() == 0)
		response->addHeader("connection", "close");
	else
	{
		response->addHeader("connection", "keep-alive");
		std::string	keepAliveValue("timeout=");
		keepAliveValue += to_string(Socket::timeLimit);
		keepAliveValue += ", max=";
		keepAliveValue += to_string(clientSock->getMaxRequest());
		response->addHeader("keep-alive", keepAliveValue);
	}
	response->makeRowString();
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

