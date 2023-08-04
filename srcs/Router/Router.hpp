#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Request.hpp"
#include "Handler/IHandler.hpp"
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
	IHandler *createHandler(const Request &request);

private:
	IHandler *_handler;
};

#endif