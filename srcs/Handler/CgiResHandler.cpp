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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


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
HttpMessage	*CgiResHandler::handleMessage(const CgiResponse &response, Socket *sock)
{
	int	wstatus;
	waitpid(-1, &wstatus, 0);
	if (WEXITSTATUS(wstatus) == 1 || response.isInvalid())
		return (new Response("500"));
	std::map<std::string, std::string> headers;
	std::cout << "type: " << response.getType() << std::endl;
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
	else if (response.getType() == CgiResponse::ClientRedirectResponseWithDocument)
	{
		std::string status = response.getHeader("Status");
		headers = response.getOtherFields();
		headers["location"] = response.getHeader("Location");
		headers["content-type"] = response.getHeader("Content-Type");
		return (new Response(status, headers, response.getBody()));
	}
	else if (response.getType() == CgiResponse::LocalRedirectResponse)
	{
		CgiSocket *cgiSock = dynamic_cast<CgiSocket *>(sock);
		if (cgiSock == NULL)
			return (new Response("500"));
		Request *req = new Request("GET", response.getHeader("Location"), "HTTP/1.1", headers, "");
		ClSocket *clSocket = cgiSock->getClSocket();
		struct sockaddr_in addr = clSocket->getLocalAddr();
		req->parsing("GET/ HTTP/1.1\r\nHost: "
		+ std::string(inet_ntoa(addr.sin_addr))
		+ ":"
		+ std::to_string(ntohs(addr.sin_port))
		+ "\r\n\r\n", 0);
		req->setAddr(clSocket);
		req->setInfo();
		return (req);
	}
	else if (response.getType() == CgiResponse::InvalidResponse)
	{
		Response *res = new Response("500");
		res->setBody(response.getBody());
		return (res);
	}
	else
		return (new Response("500"));
}
