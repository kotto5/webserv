#include "CgiResponse.hpp"
#include <string>

CgiResponse::CgiResponse(/* args */)
{
}

CgiResponse::~CgiResponse()
{
}

bool    CgiResponse::isValidLine(const std::string &line, const bool isFirstLine) const
{
    return (isHeaderField(line));
}

void	CgiResponse::setStatusLine(const std::string &line)
{
	std::string::size_type	protocol_end = line.find(" ");
	std::string::size_type	status_end = line.find(" ", protocol_end + 1);
	_protocol = line.substr(0, protocol_end);
	_status = line.substr(protocol_end + 1, status_end - protocol_end - 1);
	_statusMessage = line.substr(status_end + 1);
}

void	CgiResponse::setFirstLine(const std::string &line)
{
    setHeaderFromLine(_headers, line, ": ");
}
