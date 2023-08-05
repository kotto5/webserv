#ifndef ONLYBODY_HPP
#define ONLYBODY_HPP

#include <string>
#include "HttpMessage.hpp"

class OnlyBody : public HttpMessage
{
private:
	void			setFirstLine(const std::string &line);
public:
    OnlyBody();
    ~OnlyBody();
};

#endif