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
	void				init(Server &server, const LocationContext &lc);
	virtual Response	*handleRequest(const Request &request);
	CgiSocket			*createCgiSocket(const Request &request);
	void setClientSocket(ClSocket *clientSocket);

private:
	Server	*_server;
	ClSocket	*_clientSocket;
	const LocationContext *_locationContext;

	int runCgi(const Request &request, int pipes[2]);
	std::vector<char *> *createEnvs(const Request &request);
};

#endif