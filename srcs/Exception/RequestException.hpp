#ifndef REQUEST_EXCEPTION_HPP
#define REQUEST_EXCEPTION_HPP

#include <string>
#include "ServerException.hpp"

class RequestException : public ServerException
{
	public:
		RequestException(const std::string &status);
		~RequestException() throw();

	private:
		RequestException();
};

#endif
