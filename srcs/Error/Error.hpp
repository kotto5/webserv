#ifndef ERROR_HPP
#define ERROR_HPP

#include <map>

class Error
{
private:
    Error();
    ~Error();
public:
	static	void	print_error(std::string msg, int error_code);
	static	const	std::map<int, std::string> error_msg;
};

typedef enum E_ERROR_CODE{
    E_SYSCALL,
	E_REQ_PARSE,
	E_RES_PARSE,
	E_CONF_PARSE,
}   T_ERROR_CODE;

#endif
