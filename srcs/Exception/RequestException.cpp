#include "RequestException.hpp"

RequestException::RequestException(const std::string &status): ServerException("Request Error: " + status)
{
}

RequestException::~RequestException() throw()
{
}
