#include "Error.hpp"
#include "iostream"


void   print_error(std::string msg, int error_code) {
    if (error_code == E_SYSCALL)
        perror(msg.c_str());
    else
        std::cerr << msg << ": " << error_msg[error_code] << std::endl;
}

// _perror(request.getBody(), REQUEST_PARSE_ERROR);

const std::map<int, std::string> createErrorMap() {
    std::map<int, std::string> error_msg;

    error_msg[E_REQ_PARSE] = "Request parse error";
    error_msg[E_RES_PARSE] = "Response parse error";
    error_msg[E_CONF_PARSE] = "Config parse error";
}

const std::map<int, std::string> Error::error_msg = createErrorMap();

Error::Error() {}

Error::~Error() {}
