#include "ServerException.hpp"

ServerException::ServerException(const std::string& message): runtime_error(message)
{
}

ServerException::~ServerException() throw()
{
}
