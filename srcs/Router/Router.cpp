#include "Router.hpp"
#include "Handler/GetHandler.hpp"
#include "Handler/PostHandler.hpp"
#include "Handler/DeleteHandler.hpp"
#include "config/Config.hpp"


// Constructors
Router::Router() : _handler(NULL) {}

Router::Router(const Router &other)
{
	this->_handler = other._handler;
}

// Destructor
Router::~Router() {}

// Operators
Router &Router::operator=(const Router &rhs)
{
	if (this != &rhs)
	{
		this->_handler = rhs._handler;
	}
	return *this;
}

IHandler *Router::createHandler(const Request &request)
{
	// メソッドに対応するhandlerを取得
	std::string method = request.getMethod();

	// GETメソッドの場合
	if (method == "GET")
	{
		_handler = new GetHandler();
	}
	else if (method == "POST")
	{
		_handler = new PostHandler();
	}
	else if (method == "DELETE")
	{
		_handler = new DeleteHandler();
	}
	return _handler;
}
