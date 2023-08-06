#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "IHandler.hpp"
#include "Response.hpp"
#include <iostream>
#include <string>

class ErrorHandler : public IHandler
{
public:
	// Constructors
	ErrorHandler();
	ErrorHandler(const ErrorHandler &other);

	// Destructor
	virtual ~ErrorHandler();

	// Operators
	ErrorHandler &operator=(const ErrorHandler &rhs);

	// Member functions
	virtual Response	*handleRequest(const Request &request);
};

#endif