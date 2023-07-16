#include "Error.hpp"
#include "iostream"


void   Error::print_error(std::string msg, Error::T_ERROR_CODE error_code) {
    if (error_code == Error::E_SYSCALL)
        perror(msg.c_str());
    else
        std::cerr << msg << ": " << Error::error_msg.at(error_code) << std::endl;
}

const std::map<Error::T_ERROR_CODE, std::string> Error::createErrorMap() {
    std::map<Error::T_ERROR_CODE, std::string> error_msg;

    error_msg[Error::E_REQ_PARSE] = "Request parse error";
    error_msg[Error::E_RES_PARSE] = "Response parse error";
    error_msg[Error::E_CONF_PARSE] = "Config parse error";

    return (error_msg);
}

const std::map<Error::T_ERROR_CODE, std::string> Error::error_msg = Error::createErrorMap();

Error::Error() {}

Error::~Error() {}


// int	main(){
// 	if (open("test.txt", O_RDWR, 0666) == -1)
// 		Error::print_error("open", Error::E_SYSCALL);
// }
