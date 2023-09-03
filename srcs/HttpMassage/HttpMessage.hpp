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
	std::string							_raw;
	std::string							_protocol;
	std::map<std::string, std::string>	_headers;
	std::string							_body;

	bool								_isHeaderEnd;
	bool								_isBodyEnd;
	bool								_isBadFormat;
	bool								_isChunked;
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
	void						decodeChunked(std::string &body);

public:
	HttpMessage();
	HttpMessage(const std::string &_raw);
	virtual ~HttpMessage();

	int							parsing(const std::string &row, const std::size_t limitClientMsgSize);
	bool						doesSendEnd() const;
	void						addSendPos(std::size_t pos);
	void						printHeader() const;
	bool						isTooBigError() const;
	bool						isCompleted() const;

	const std::string  			&getRaw() const;
	const std::string			&getProtocol() const;
	const std::string  			&getHeader(std::string key) const;
	void						addHeader(std::string key, std::string value);
	const std::string			&getBody() const;
	const uint8_t				*getSendBuffer();
	std::size_t					getContentLength() const;
	std::size_t					getContentLengthRemain() const ;
	bool						getIsBadFormat() const;
	bool						isInvalid() const;

	virtual	void				setFirstLine(const std::string &line) = 0;
	static void					setHeader(std::map<std::string, std::string>& m, std::string first, std::string second);
	static void					setHeaderFromLine(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword);
	void						setContentLength();
	void						setBody(const std::string &addBody);
	void						setBodyEnd(bool isBodyEnd);
};

#endif