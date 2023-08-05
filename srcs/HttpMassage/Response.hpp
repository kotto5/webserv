#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <map>
#include <string>
#include "HttpMessage.hpp"
class Response : public HttpMessage
{
public:
	// Constructors
	Response(const std::string &status, std::map<std::string, std::string> headers, const std::string &body);
	Response(const std::string &status);
	Response(const Response &other);
	Response();

	// Destructor
	~Response();

	// Operators
	Response &operator=(const Response &rhs);

	// Member functions
	std::string toString() const;
	static std::string getMimeType(const std::string &filename);

	// Getters/setters
	const std::string	&getStatus() const;

private:
	// ステータスコード
	std::string _status;
	std::string _statusMessage;

	// 拡張子とMIMEタイプの対応表
	static std::map<std::string, std::string> extensionToMime;

	// ステータスコードから該当するメッセージを取得する
	const std::string	&getStatusMessage(const std::string &statusCode) const;

	// 現在日時をセットする
	void setDate();

	// Content-Lengthをセットする
	void setContentLength();

	// Serverをセットする
	void setServer();

	void	setFirstLine(const std::string &line);
};

#endif