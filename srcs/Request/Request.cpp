#include "Request.hpp"

// Constructors
Request::Request(const std::string &method, const std::string &uri, const std::string &protocol,
				 const std::map<std::string, std::string> &headers, const std::string &body)
	: _method(method), _uri(uri), _protocol(protocol), _headers(headers), _body(body)
{
}

Request::Request(const Request &other)
{
	*this = other;
}

// Destructor
Request::~Request() {}

// Operators
Request &Request::operator=(const Request &rhs)
{
	if (this != &rhs)
	{
		this->_method = rhs._method;
		this->_uri = rhs._uri;
		this->_protocol = rhs._protocol;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
	}
	return *this;
}

// Getters/Setters
std::string Request::getMethod() const
{
	return this->_method;
}

std::string Request::getUri() const
{
	return this->_uri;
}

std::string Request::getProtocol() const
{
	return this->_protocol;
}

std::string Request::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);
	if (it != this->_headers.end())
		return it->second;
	return "";
}

std::string Request::getBody() const
{
	return this->_body;
}

// Not use
Request::Request() : _method(), _uri(), _headers(), _body() {}