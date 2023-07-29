#ifndef SERVER_EXCEPTION_HPP
#define SERVER_EXCEPTION_HPP

#include <stdexcept>

class ServerException: public std::runtime_error
{
public:
	ServerException(const std::string& message);
	~ServerException() throw();
};

#endif
