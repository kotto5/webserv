#include "Router.hpp"
#include "../Handler/GetHandler.hpp"
#include "../config/Config.hpp"

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
	// std::string alias;
	// try
	// {
		// Serverブロックを取得
		// const ServerContext &serverContext =
		// 	Config::getInstance()->getHTTPBlock().getServerContexts(80, request.getHeader("host"));
		// const LocationContext &locationContext = serverContext.getLocationContext(request.getUri());
		// alias = locationContext.getAlias();
		// request.setacutualUri(alias, root);
		// エイリアスがある場合は置き換える
	// }
	// catch (std::runtime_error &e)
	// {
	// 	// 一致するロケーションブロックがない場合は404を返す
	// 	std::cout << e.what() << std::endl;
	// }

	// メソッドに対応するhandlerを取得
	std::string method = request.getMethod();

	// GETメソッドの場合
	if (method == "GET")
	{
		_handler = new GetHandler();
	}
	return _handler;
}
