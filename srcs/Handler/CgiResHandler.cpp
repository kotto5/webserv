#include "Config.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "Response.hpp"
#include "HttpMessage.hpp"
#include "RequestException.hpp"
#include "Logger.hpp"
#include "CgiResHandler.hpp"
#include <sys/wait.h>

// Constructors
CgiResHandler::CgiResHandler()
{
	// 規定値を200に設定
	this->_status = "";
}

CgiResHandler::CgiResHandler(const CgiResHandler &other)
{
	*this = other;
}

// Destructor
CgiResHandler::~CgiResHandler() {}

// Operators
CgiResHandler &CgiResHandler::operator=(const CgiResHandler &rhs)
{
	if (this != &rhs)
	{
		this->_status = rhs._status;
	}
	return *this;
}

/**
 * @brief GETメソッドの処理を行う
 *
 * @param request リクエスト
 * @return Response レスポンス
 */
HttpMessage	*CgiResHandler::handleMessage(const Response &response)
{
	int	wstatus;
	waitpid(-1, &wstatus, 0);
	if (WEXITSTATUS(wstatus) == 1 || response.isInvalid())
		return (new Response("500"));
	return (new Response(response));
}
