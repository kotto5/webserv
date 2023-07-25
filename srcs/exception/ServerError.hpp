#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <stdexcept>

class ServerError: public std::runtime_error
{
public:
	ServerError(const std::string& message);
	~ServerError();
};

#endif

