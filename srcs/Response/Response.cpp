#include "Response.hpp"
#include "HettpStatus.hpp"

// Constructors
Response::Response(const int status, std::map<std::string, std::string> headers,
				   const std::string &body)
	: _status(status), _headers(headers), _body(body)
{
}

Response::Response(const Response &other)
{
	this->_body = other._body;
	this->_headers = other._headers;
	this->_body = other._body;
}

// Destructor
Response::~Response() {}

// Operators
Response &Response::operator=(const Response &rhs)
{
	if (this != &rhs)
	{
		this->_status = rhs._status;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
	}
	return *this;
}

// Getters/setters
int Response::getStatus() const
{
	return this->_status;
}

std::string Response::getHeader(const std::string &key) const
{
	return this->_headers.at(key);
}

std::string Response::getBody() const
{
	return this->_body;
}

/**
 * @brief ステータスコードに対応するメッセージを返す
 *
 * @param statusCode ステータスコード
 * @return std::string ステータスメッセージ
 */
std::string Response::getStatusMessage(int statusCode) const
{
	return HttpStatus::HTTP_STATUS.at(statusCode);
}

/**
 * @brief レスポンス内容を平文に変換する。
 *
 * @return std::string レスポンス内容
 */
std::string Response::toString() const
{
	std::string response;

	// ステータス行
	response += "HTTP/1.1 " + std::to_string(this->_status) + " " +
				getStatusMessage(this->_status) + "\r\n";

	// ヘッダー
	std::map<std::string, std::string>::const_iterator iter;
	for (iter = this->_headers.begin(); iter != this->_headers.end(); ++iter)
	{
		const std::pair<std::string, std::string> &pair = *iter;
		response += pair.first + ": " + pair.second + "\r\n";
	}

	// 終了行
	response += "\r\n";

	// ボディ
	response += this->_body;
	return response;
}

Response::Response() {}
