#include "Router.hpp"
#include "../Handler/GetHandler.hpp"
#include <assert.h>

// Constructors
Router::Router(): _handler(NULL){}

Router::Router(const Router &other)
{
	this->_handler = other._handler;
}

// Destructor
Router::~Router(){}

// Operators
Router & Router::operator=(const Router &rhs)
{
	if(this != &rhs)
	{
		this->_handler = rhs._handler;
	}
	return *this;
}

IHandler *Router::findHandler(const Request &request)
{
	// configの情報を取得
	// パスが存在するか確認

	// メソッドに対応するhandlerを取得
	std::string method = request.getMethod();

	// GETメソッドの場合
	if (method == "GET")
	{
		_handler = new GetHandler();
	}
	return _handler;
}

