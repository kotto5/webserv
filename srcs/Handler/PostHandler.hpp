#ifndef POSTHANDLER_HPP
#define POSTHANDLER_HPP

#include "IHandler.hpp"
#include "Response.hpp"
#include <iostream>
#include <string>

class PostHandler : public IHandler
{
private:
	std::string	getUniqueFileNameWithPrefix(const std::string &dir, const std::string &file);

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