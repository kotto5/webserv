#ifndef ERROR_HPP
#define ERROR_HPP

#include <map>

class Error
{
private:
    Error();
    ~Error();
public:
	typedef enum E_ERROR_CODE{
		E_SYSCALL,
		E_REQ_PARSE,
		E_RES_PARSE,
		E_CONF_PARSE,
	}   T_ERROR_CODE;

	static const	std::map<Error::T_ERROR_CODE, std::string> createErrorMap();
	static	void	print_error(std::string msg, T_ERROR_CODE error_code);
	static	const	std::map<T_ERROR_CODE, std::string> error_msg;
};


#endif
