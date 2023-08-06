#ifndef REQUEST_EXCEPTION_HPP
#define REQUEST_EXCEPTION_HPP

#include <string>
#include "ServerException.hpp"

class RequestException : public ServerException
{
public:
	RequestException(const std::string &status);
	~RequestException() throw();
	const std::string &getStatus() const;

private:
	std::string _status;
	RequestException();
};

#endif
