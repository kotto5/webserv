#ifndef POSTHANDLER_HPP
#define POSTHANDLER_HPP

#include "IHandler.hpp"
#include "Response.hpp"
#include <iostream>
#include <string>

class PostHandler : public IHandler
{
public:
	// Constructors
	PostHandler();
	PostHandler(const PostHandler &other);

	// Destructor
	virtual ~PostHandler();

	// Operators
	PostHandler &operator=(const PostHandler &rhs);

	// Member functions
	virtual HttpMessage	*handleRequest(const Request &request, const ServerContext &serverContext);
};

#endif