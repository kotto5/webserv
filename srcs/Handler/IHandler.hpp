#ifndef IHANDLER_HPP
#define IHANDLER_HPP

#include "../Response/Response.hpp"
#include "../Request/Request.hpp"
#include <iostream>
#include <string>

class IHandler
{
public:
	// Destructor
	virtual ~IHandler() {};

	// Member function
	virtual Response	handleRequest(const Request &request) = 0;
protected:
	int _status;
};

#endif