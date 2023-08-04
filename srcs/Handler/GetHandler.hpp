#ifndef GETHANDLER_HPP
#define GETHANDLER_HPP

#include "IHandler.hpp"
#include "Response.hpp"
#include <iostream>
#include <string>

class GetHandler : public IHandler
{
public:
	// Constructors
	GetHandler();
	GetHandler(const GetHandler &other);

	// Destructor
	virtual ~GetHandler();

	// Operators
	GetHandler &operator=(const GetHandler &rhs);

	// Member functions
	virtual Response	*handleRequest(const Request &request);
};

#endif