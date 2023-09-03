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
#include "CgiResponse.hpp"

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

// Status: 200 OK
// Location: /path/to/redirect
	// localLocation
// Location: http://ejxample.com/path/to/redirect
	// clientLocation
// Content-Type: text/html

    //   header-field    = CGI-field | other-field
    //   CGI-field       = Content-Type | Location | Status
    //   other-field     = protocol-field | extension-field
    //   protocol-field  = generic-field
    //   extension-field = generic-field
    //   generic-field   = field-name ":" [ field-value ] NL
    //   field-name      = token
    //   field-value     = *( field-content | LWSP )
    //   field-content   = *( token | separator | quoted-string )

/**
 * @brief GETメソッドの処理を行う
 *
 * @param request リクエスト
 * @return Response レスポンス
 */
HttpMessage	*CgiResHandler::handleMessage(const CgiResponse &response)
{
	int	wstatus;
	waitpid(-1, &wstatus, 0);
	if (WEXITSTATUS(wstatus) == 1 || response.isInvalid())
		return (new Response("500"));
	std::map<std::string, std::string> headers;
	if (response.getType() == CgiResponse::DocumentResponse)
	{
		headers = response.getOtherFields();
		headers["content-type"] = response.getHeader("Content-Type");

		std::string status = response.getHeader("Status");
		if (status != "")
			return (new Response(status, headers, response.getBody()));
		else
			return (new Response("200", headers, response.getBody()));
	}
	else if (response.getType() == CgiResponse::ClientRedirectResponse)
	{
		headers["location"] = response.getHeader("Location");
		return (new Response("302", headers, ""));
	}
	// else if (response.getType() == CgiResponse::LocalRedirectResponse)
	// {
	// 	const std::string &location = response.getHeader("Location");
	// }
	// else if (response.getType() == CgiResponse::Type::ErrorResponse)
	// {
	// 	Response *res = new Response("500");
	// 	res->setBody(response.getBody());
	// 	return (res);
	// }
	else
		return (new Response("500"));
}
