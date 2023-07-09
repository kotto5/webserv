#include "Response.hpp"
#include "HttpStatus.hpp"
#include "MimeType.hpp"
#include <ctime>

// Constructors
Response::Response(const int status, std::map<std::string, std::string> headers,
				   const std::string &body)
	: _status(status), _headers(headers), _body(body)
{
	// ヘッダーに日付を追加
	setDate();
	// ヘッダーにContent-Lengthを追加
	setContentLength();
	// ヘッダーにServerを追加
	setServer();
}

Response::Response(const Response &other)
{
	this->_body = other._body;
	this->_headers = other._headers;
	this->_status = other._status;
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
 * @brief レスポンス内容を平文に変換する
 *
 * @return std::string レスポンス内容
 */

#include <unistd.h>

std::string Response::toString() const
{
	std::string response;

	write(1, "a\n", 2);
	// ステータス行を平文に変換
	std::to_string(this->_status);
	write(1, "a1\n", 3);
	getStatusMessage(this->_status);
	write(1, "a2\n", 3);

	response += "HTTP/1.1 " + std::to_string(this->_status) + " " +
				getStatusMessage(this->_status) + "\r\n";
	write(1, "b\n", 2);

	// ヘッダーを平文に変換
	std::map<std::string, std::string>::const_iterator iter;
	write(1, "c\n", 2);
	for (iter = this->_headers.begin(); iter != this->_headers.end(); ++iter)
	{
		const std::pair<std::string, std::string> &pair = *iter;
		response += pair.first + ": " + pair.second + "\r\n";
	}
	write(1, "d\n", 2);

	// 終了行を追加
	response += "\r\n";

	write(1, "e\n", 2);
	// ボディを追加
	response += this->_body;
	return response;
}

/**
 * @brief 現在日時をセットする
 *
 * @detail RFCに準拠し、Dateフィールドは以下の形式とする。
 * 		Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
 */
void Response::setDate()
{
	char buf[100];

	std::time_t t = std::time(0);
	std::tm *tm_gm = gmtime(&t);
	// HTTPの形式に変換
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", tm_gm);
	std::string dateNow(buf);
	this->_headers["Date"] = dateNow;
}

/**
 * @brief Content-Lengthをセットする
 *
 * @detail Content-Lengthフィールドは、ボディの長さをバイト単位で表す。
 *
 */
void Response::setContentLength()
{
	this->_headers["Content-Length"] = std::to_string(this->_body.length());
}

void Response::setServer()
{
	this->_headers["Server"] = "Webserv 0.1";
}

/**
 * @brief 拡張子から該当するMIMEタイプを取得する
 *
 * @detail Handlerクラスから参照されるため静的メソッドとする
 * @param filename ファイル名
 * @return std::string mimeタイプ
 */
std::string Response::getMimeType(const std::string &filename)
{
	// 拡張子を取得
	std::string::size_type pos = filename.rfind(".");
	if (pos == std::string::npos)
	{
		// 拡張子が存在しない場合はoctet-streamを返す
		return "application/octet-stream";
	}
	std::string extension = filename.substr(pos);

	// MIMEタイプを判別
	if (MimeType::MIME_TYPE.find(extension) != MimeType::MIME_TYPE.end())
	{
		// 該当するMIMEが存在する場合はそれを返す
		return MimeType::MIME_TYPE.at(extension);
	}
	// 該当するMIMEが存在しない場合はoctet-streamを返す
	return "application/octet-stream";
}

// Not use
Response::Response() {}
