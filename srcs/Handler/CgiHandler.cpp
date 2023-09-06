#include "CgiHandler.hpp"
#include "ServerException.hpp"
#include "utils.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include <filesystem>
#include "CgiResponse.hpp"
#include <sstream>

// Constructors
CgiHandler::CgiHandler()
{
	_status = "200";
}

CgiHandler::CgiHandler(const CgiHandler &other)
{
	*this = other;
}

// Destructor
CgiHandler::~CgiHandler() {}

// Operators
CgiHandler &CgiHandler::operator=(const CgiHandler &rhs)
{
	if (this != &rhs)
	{
		_status = rhs._status;
	}
	return *this;
}

/**
 * @brief CGIの呼び出し処理を行う
 *
 * @param request
 * @return Response*
 */
HttpMessage *CgiHandler::handleRequest(const Request &request, const ServerContext &serverContext)
{
	Request *req = new(std::nothrow) Request(request.getBody());
	if (req == NULL)
		return handleError("500", serverContext);
	return (req);
}
