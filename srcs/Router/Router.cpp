#include "Router.hpp"
#include "Config.hpp"
#include "GetHandler.hpp"
#include "PostHandler.hpp"
#include "DeleteHandler.hpp"
#include "ErrorHandler.hpp"

#include "RequestException.hpp"


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
	Response *response = NULL;

	// メソッドに対応するhandlerを取得
	std::string method = request.getMethod();
	IHandler *handler = _handlers.at(method);

	try
	{
		response = handler->handleRequest(request);
	}
	catch (const RequestException &status)
	{
		response = _errorHandler.handleRequest(request);
	}
	return response;
}
