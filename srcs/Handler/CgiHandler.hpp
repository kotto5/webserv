#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "IHandler.hpp"
#include "Response.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include <iostream>

enum {
	S_CHILD = 0,
	S_PARENT = 1,
};

class CgiHandler : public IHandler
{
public:
	// Constructors
	CgiHandler(Server *server);
	CgiHandler(Socket clientSocket);
	CgiHandler(const CgiHandler &other);

	// Destructor
	virtual ~CgiHandler();

	// Operators
	CgiHandler &operator=(const CgiHandler &rhs);

	// Member functions
	virtual Response	*handleRequest(const Request &request);
	void setClientSocket(Socket *clientSocket);

private:
	Server	*_server;
	Socket	*_clientSocket;
	int runCgi(const Request &request, int pipes[2][2]);
	std::vector<char *> createEnvs(const Request &request);
};

#endif