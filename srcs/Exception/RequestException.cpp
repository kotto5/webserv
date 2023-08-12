#include "RequestException.hpp"

RequestException::RequestException(const std::string &status): ServerException("Request Error: " + status)
{
	_status = status;
}

RequestException::~RequestException() throw()
{
}

const std::string &RequestException::getStatus() const
{
	return _status;
}