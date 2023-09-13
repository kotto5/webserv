#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Request.hpp"
#include "IHandler.hpp"
#include "GetHandler.hpp"
#include "PostHandler.hpp"
#include "DeleteHandler.hpp"
#include "Server.hpp"
#include "ServerContext.hpp"
#include <iostream>
#include <string>
#include "CgiHandler.hpp"

class Router
{
public:
	// Constructors
	Router();
	Router(const Router &other);
	// Destructor
	~Router();

	// Operators
	Router &operator=(const Router &rhs);

	// Member functions
	HttpMessage *routeHandler(HttpMessage &request, Socket *sock);

private:
	static bool	isAllowedMethod(const std::string &method, const LocationContext &locationContext);
	int			getRequestError(const Request *request, const LocationContext &locationContext);
	Response	*handleError(const std::string &status, const ServerContext &serverContext);
	std::string generateDefaultErrorPage();
	HttpMessage *makeSendMessage(HttpMessage &message, Socket *sock);
	void		addKeepAliveHeader(Response *response, ClSocket *clientSock);

	//　登録したハンドラーのリスト
	std::map<std::string, IHandler *> _handlers;

	GetHandler _getHandler;
	PostHandler _postHandler;
	DeleteHandler _deleteHandler;
	CgiHandler _cgiHandler;

	Server *_server;
};

#endif