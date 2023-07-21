#include "Exception.hpp"

Exception::Exception(const std::string& message)
	: std::runtime_error(message)
{
}

Exception::~Exception()
{
}
