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
	bool								_tooBigError;
	static	std::string					_empty;

	std::string::size_type				_readPos;
	std::size_t							_sendPos;
	uint8_t								*_sendBuffer;
	std::size_t							_sendBufferSize;
	bool								_doesSendEnd;

	std::string::size_type				_contentLength;

	bool						isValidLine(const std::string &line, const bool isFirstLine) const;
	static std::string			makeHeaderKeyLower(std::string key);

public:
	HttpMessage();
	HttpMessage(const std::string &_row);
	// HttpMessage(const std::string &protocol, std::map<std::string, std::string> headers, const std::string &body);
	virtual ~HttpMessage();


	int							parsing(const std::string &row, const std::size_t limitClientMsgSize);
	bool						isEnd() const;
	void						addSendPos(std::size_t pos);
	bool						doesSendEnd() const;
	void						printHeader() const;
	bool						isBadRequest() const;
	bool						isTooBigError() const;

	const std::string  			&getRow() const;
	const std::string			&getProtocol() const;
	const std::string  			&getHeader(std::string key) const;
	const std::string			&getBody() const;
	const uint8_t				*getSendBuffer();
	std::size_t					getContentLength() const;
	std::size_t					getContentLengthRemain() const ;
	virtual	void				setFirstLine(const std::string &line) = 0;
	static void					setHeader(std::map<std::string, std::string>& m, std::string first, std::string second);
	static void					setHeaderFromLine(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword);
	void						setContentLength();
	void						setBody(const std::string &addBody);
};

#endif