#include "Request.hpp"

// Constructors
Request::Request(){}

Request::Request(const Request &other)
{
	this->_method = other._method;
	this->_uri = other._uri;
	this->_headers = other._headers;
	this->_body = other._body;
}

// Destructor
Request::~Request(){}

// Operators
Request & Request::operator=(const Request &rhs)
{
	if (this != &rhs) {
		this->_method = rhs._method;
		this->_uri = rhs._uri;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
	}
	return *this;
}
