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
	Response *routeHandler(const Request &request, Socket *sock = NULL);
	Response *handleError(const Request &request, const std::string &status);

private:
	bool		isRedirect(const Request &request) const;
	std::string generateDefaultErrorPage();
	bool	isAllowedMethod(const Request& request) const;
	bool	requestWantsCgi(const Request &request);

	//　登録されたハンドラーのリスト
	std::map<std::string, IHandler *> _handlers;

	GetHandler _getHandler;
	PostHandler _postHandler;
	DeleteHandler _deleteHandler;

	Server *_server;
	const ServerContext *_serverContext;
};

#endif