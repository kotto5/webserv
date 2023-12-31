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
    (void)isFirstLine;
    return (isHeaderField(line));
}


void	CgiResponse::setFirstLine(const std::string &line)
{
    setHeaderFromLine(_headers, line, ": ");
}

// リダイレクトする == location を持つ
// body がある == content-type を持つ

bool    CgiResponse::isLocalPathQuery(const std::string &location) const
{
    return (location[0] == '/');
}

bool    CgiResponse::isCgiField(const std::string &headerKey) const
{
    // headerKey is Lowercase!
    if (headerKey == "status")
        return (true);
    else if (headerKey == "location")
        return (true);
    else if (headerKey == "content-Type")
        return (true);
    else
        return (false);
}

bool    CgiResponse::isOtherField(const std::string &headerKey) const
{
    return (isCgiField(headerKey) == false);
}

bool    CgiResponse::isProtocolField(const std::string &headerKey) const
{
    (void)headerKey;
    return (false);
}

bool    CgiResponse::isClientRedirectResponse() const
{
    std::map<std::string, std::string>::const_iterator it = _headers.begin();
    if (getHeader("location").empty() || 
        getHeader("content-type").empty() == false ||
        getHeader("status").empty() == false)
        return (false);
    for (; it != _headers.end(); ++it)
    {
        if (isProtocolField(it->first) == true)
            return (false);
    }
    return (true);
}

// ○ .. must  △ .. may  x .. must not
//                           |   content-type  status  location  other-field
// document-response         |        o          △        x           o
// local-redir-response      |        x          x        o           x
// client-redir-response     |        x          x        o           △ ※1
// client-redir-doc-response |        o          o        o           o

// ※1 other-field には extension-field のみ許される

CgiResponse::Type    CgiResponse::getType() const
{
    const std::string &location = getHeader("location");
    const std::string &contentType = getHeader("content-type");
    const std::string &status = getHeader("status");
    if (location.empty())
    {
        if (contentType.empty())
            return (InvalidResponse);
        else
            return (DocumentResponse);
    }
    else
    {
        if (isLocalPathQuery(location))
        {
            if (_headers.size() == 1)
                return (LocalRedirectResponse);
            else
                return (InvalidResponse);
        }
        if (isClientRedirectResponse())
            return (ClientRedirectResponse);
        else if (contentType.empty() == false && status.empty() == false)
            return (ClientRedirectResponseWithDocument);
        else
            return (InvalidResponse);
    }
}

std::map<std::string, std::string> CgiResponse::getOtherFields() const{
    std::map<std::string, std::string> otherFields;
    std::map<std::string, std::string>::const_iterator it = _headers.begin();
    for (; it != _headers.end(); ++it)
    {
        if (isOtherField(it->first) == true)
            otherFields[it->first] = it->second;
    }
    return (otherFields);
}
