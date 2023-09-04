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
	Router(Server &server);
	Router(const Router &other);

	Router();
	// Destructor
	~Router();

	// Operators
	Router &operator=(const Router &rhs);

	// Member functions
	HttpMessage *routeHandler(HttpMessage &request, Socket *sock = NULL);
	Response *handleError(const Request &request, const std::string &status);

private:
	bool		isRedirect(const Request &request) const;
	static bool	isAllowedMethod(const std::string &method, const LocationContext &locationContext);
	bool		isConnectionCgi(const Request &request);
	int			getRequestError(const Request *request, const LocationContext &locationContext);
	Response	*handleError(const std::string &status, const LocationContext &locationContext);
	std::string generateDefaultErrorPage();

	//　登録したハンドラーのリスト
	std::map<std::string, IHandler *> _handlers;

	GetHandler _getHandler;
	PostHandler _postHandler;
	DeleteHandler _deleteHandler;
	CgiHandler _cgiHandler;

	Server *_server;
	const ServerContext *_serverContext;
};

#endif