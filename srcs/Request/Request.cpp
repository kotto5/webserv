#include "Request.hpp"

// Constructors
Request::Request(const std::string &method, const std::string &uri,
				 const std::map<std::string, std::string> &headers, const std::string &body)
	: _method(method), _uri(uri), _headers(headers), _body(body)
{
}

Request::Request(const Request &other)
{
	this->_method = other._method;
	this->_uri = other._uri;
	this->_headers = other._headers;
	this->_body = other._body;
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

std::string Request::getHeader(const std::string &key) const
{
	return this->_headers.find(key)->second;
}

std::string Request::getBody() const
{
	return this->_body;
}

// Not use
Request::Request() : _method(), _uri(), _headers(), _body() {}