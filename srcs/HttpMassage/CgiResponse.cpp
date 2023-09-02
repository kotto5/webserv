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


void	CgiResponse::setFirstLine(const std::string &line)
{
    setHeaderFromLine(_headers, line, ": ");
}
