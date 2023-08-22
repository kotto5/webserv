#ifndef CGIRESHANDLER_HPP
#define CGIRESHANDLER_HPP

#include "IHandler.hpp"
#include "Response.hpp"
#include <iostream>
#include <string>

class CgiResHandler
{
private:
    std::string	_status;
public:
    // Constructors
    CgiResHandler();
    CgiResHandler(const CgiResHandler &other);

    // Destructor
    virtual ~CgiResHandler();

    // Operators
    CgiResHandler &operator=(const CgiResHandler &rhs);

    // Member functions
    virtual HttpMessage	*handleMessage(const Response &response);
};

#endif