#ifndef HTTPSTATUS_HPP
#define HTTPSTATUS_HPP

#include <iostream>
#include <string>
#include <map>

class HttpStatus
{
public:
	static const std::map<int, std::string> HTTP_STATUS;
};

#endif