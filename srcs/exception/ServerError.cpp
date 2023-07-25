#include "ServerError.hpp"

ServerError::ServerError(const std::string& message): runtime_error(message)
{
}

ServerError::~ServerError()
{
}
