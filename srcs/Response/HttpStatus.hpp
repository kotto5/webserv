#ifndef HTTPSTATUS_HPP
#define HTTPSTATUS_HPP

#include <map>
#include <string>

class HttpStatus
{
public:
	static const std::map<int, std::string> HTTP_STATUS;
};

#endif