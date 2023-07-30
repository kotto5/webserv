#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <iostream>
#include <map>
#include <string>
#include "Socket.hpp"

class HttpMessage
{
protected:
	std::string							_protocol;
	std::map<std::string, std::string>	_headers;
	std::string							_body;
	std::string							_readBuffer;
	std::string::size_type				_readPos;
	bool								_isHeaderEnd;
	bool								_isBodyEnd;
	static	std::string					_empty;

	virtual	void				setFirstLine(const std::string &line) = 0;
	static void					setHeaderToLower(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword);
	void						setBody(const std::string &row);
	bool						isValidLine(const std::string &line, const bool isFirstLine) const;

public:
	HttpMessage();
	virtual ~HttpMessage();

	const std::string   &getRow() const;
	const std::string	&getHeader(const std::string &key) const;
	const std::string	&getBody() const;
	const std::string	&getProtocol() const;

	int		   	parsing(const std::string &row);
	bool	isEnd() const;
};

#endif