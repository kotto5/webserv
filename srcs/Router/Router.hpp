#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "../Request/Request.hpp"
#include "../Handler/IHandler.hpp"
#include <iostream>
#include <string>

class Router
{
public:
	// Constructors
	Router();
	Router(const Router &copy);

	// Destructor
	~Router();

	// Operators
	Router &operator=(const Router &assign);

	// Member functions
	IHandler *findHandler(const Request &request);

private:
	IHandler *_handler;
};

#endif