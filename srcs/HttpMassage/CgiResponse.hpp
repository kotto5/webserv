#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include "HttpMessage.hpp"
#include <string>

// cgi handler でいつか使うリスト　とっておいて

// document-response = Content-Type [ Status ] *other-field NL response-body
// local-redir-response = local-Location NL
// client-redir-response = client-Location *extension-field NL
// client-redirdoc-response = client-Location Status Content-Type *other-field NL response-body

// local-Location  = "Location:" local-pathquery NL
// client-Location = "Location:" fragment-URI NL

// client redir-doc は location status content-type body がある
// client location は location があり、body がない
// local-redir-response は location がある
// client location と local-redir-response の違いは 

    // enum Type
    // {
    //     DocumentResponse,
    //     LocalRedirectResponse,
    //     ClientRedirectResponse,
    //     ClientRedirectResponseWithDocument
    // };
    // Type _type;


class CgiResponse: public HttpMessage
{
private:
	void	setFirstLine(const std::string &line);
	bool	isValidLine(const std::string &line, const bool isFirstLine) const;
    void	setStatusLine(const std::string &line);
	std::string _protocol;
	std::string _status;
	std::string	_statusMessage;
public:
    CgiResponse(/* args */);
    ~CgiResponse();
};

#endif