#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "IHandler.hpp"
#include "Response.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "LocationContext.hpp"
#include <iostream>

enum {
	S_CHILD = 0,
	S_PARENT = 1,
};

class CgiHandler : public IHandler
{
public:
	// Constructors
	CgiHandler();
	CgiHandler(Socket clientSocket);
	CgiHandler(const CgiHandler &other);

	// Destructor
	virtual ~CgiHandler();

	// Operators
	CgiHandler &operator=(const CgiHandler &rhs);

	// Member functions
	virtual HttpMessage	*handleRequest(const Request &request, const ServerContext &serverContext);
private:
};

#endif