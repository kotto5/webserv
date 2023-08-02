#ifndef DELETEHANDLER_HPP
#define DELETEHANDLER_HPP

#include "IHandler.hpp"
#include "Response.hpp"
#include <iostream>
#include <string>

class DeleteHandler : public IHandler
{
public:
	// Constructors
	DeleteHandler();
	DeleteHandler(const DeleteHandler &other);

	// Destructor
	virtual ~DeleteHandler();

	// Operators
	DeleteHandler &operator=(const DeleteHandler &rhs);

	// Member functions
	virtual Response	*handleRequest(const Request &request);
};

#endif