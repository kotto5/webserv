#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <iostream>
#include <map>
#include <string>
#include "Socket.hpp"
#include <vector>

class HttpMessage
{
protected:
	std::string							_row;
	std::string							_protocol;
	std::map<std::string, std::string>	_headers;
	std::string							_body;

	bool								_isHeaderEnd;
	bool								_isBodyEnd;
	static	std::string					_empty;

	std::string::size_type				_readPos;
	std::size_t							_sendPos;
	uint8_t								*_sendBuffer;
	std::size_t							_sendBufferSize;
	bool								_doesSendEnd;

	std::size_t							_contentLength;

	virtual	void				setFirstLine(const std::string &line) = 0;
	static void					setHeaderToLower(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword);
	void						setBody(const std::string &row);
	bool						isValidLine(const std::string &line, const bool isFirstLine) const;

public:
	HttpMessage();
	// HttpMessage(const std::string &protocol, std::map<std::string, std::string> headers, const std::string &body);
	virtual ~HttpMessage();

	const std::string   &getRow() const;
	const std::string	&getHeader(const std::string &key) const;
	const std::string	&getBody() const;
	const std::string	&getProtocol() const;

	int							parsing(const std::string &row, const bool inputClosed);
	bool						isEnd() const;
	void						addSendPos(std::size_t pos);
	const uint8_t				*getSendBuffer();
	bool						doesSendEnd() const;
	std::size_t					getContentLength() const;
	void						setContentLength();
	std::size_t					getContentLengthRemain() const ;
	void						printHeader() const;
	bool						isBadRequest() const;
	bool						isTooBigError() const;
};

#endif