#ifndef ERROR_HPP
#define ERROR_HPP

#include <map>

class Error
{
private:
    Error();
    ~Error();
public:

static void   print_error(std::string msg, int error_code);
static std::map<int, std::string> error_msg;
};

Error::Error() {}

Error::~Error() {}



typedef enum {
    SYSERROR = 0,
    REQUEST_PARSE_ERROR,
}   error_code;

std::map<int, std::string> error_msg = {
    {REQUEST_PARSE_ERROR, "Request parse error"},

};


#endif
