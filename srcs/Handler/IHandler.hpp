#ifndef IHANDLER_HPP
#define IHANDLER_HPP

#include "Response.hpp"
#include "Request.hpp"
#include "LocationContext.hpp"
#include <iostream>
#include <string>

class IHandler
{
public:
	// Destructor
	virtual ~IHandler() {};

	// Member function
	virtual HttpMessage	*handleRequest(const Request &request, const ServerContext &serverContext) = 0;
	static	HttpMessage	*handleError(const std::string &status, const ServerContext &serverContext);
protected:
	std::string	_status;
	LocationContext _locationContext;
};

#endif