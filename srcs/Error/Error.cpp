#include "Error.hpp"
#include "iostream"


void   print_error(std::string msg, int error_code) {
    if (error_code == SYSERROR)
        perror(msg.c_str());
    else
        std::cerr << msg << ": " << error_msg[error_code] << std::endl;
}

_perror(request.getBody(), REQUEST_PARSE_ERROR);
