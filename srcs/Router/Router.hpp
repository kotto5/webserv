#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Request.hpp"
#include "Handler/IHandler.hpp"
#include "GetHandler.hpp"
#include "PostHandler.hpp"
#include "DeleteHandler.hpp"
#include "ErrorHandler.hpp"

#include <iostream>
#include <string>

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
	Response *routeHandler(const Request &request);

private:
	//　登録されたハンドラーのリスト
	std::map<std::string, IHandler *> _handlers;

	GetHandler _getHandler;
	PostHandler _postHandler;
	DeleteHandler _deleteHandler;
	ErrorHandler _errorHandler;
};

#endif