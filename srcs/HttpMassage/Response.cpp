#include "Response.hpp"
#include "HttpStatus.hpp"
#include "MimeType.hpp"
#include <ctime>
#include "utils.hpp"

Response::Response() {
	_raw = "";
}

Response::Response(const std::string &status, std::map<std::string, std::string> headers,
				   const std::string &body)
{
	std::string::size_type pos = status.find(' ');
	if (pos != std::string::npos)
	{
		_status = status.substr(0, pos);
		_statusMessage = status.substr(pos + 1);
	}
	else
	{
		_status = status;
		_statusMessage = getStatusMessage(status);
	}
	_headers = headers;
	_body = body;
	// ヘッダーに日付を追加
	setDate();
	setDateToCookie();
	// ヘッダーにContent-Lengthを追加
	setContentLength();
	// ヘッダーにServerを追加
	setServer();
	makeRowString();
}

Response::Response(const std::string &status)
	: _status(status)
{
	_body = "";
	setDate();
	setContentLength();
	setServer();
	makeRowString();
}


Response::Response(const Response &other): HttpMessage()
{
	this->_status = other._status;
	this->_protocol = other._protocol;
	this->_headers = other._headers;
	this->_body = other._body;
	_raw = other._raw;
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

const std::string   &Response::getStatus() const
{
	return this->_status;
}

/**
 * @brief ステータスコードに対応するメッセージを返す
 *
 * @param statusCode ステータスコード
 * @return std::string ステータスメッセージ
 */
const std::string	&Response::getStatusMessage(const std::string &statusCode) const
{
	return HttpStatus::HTTP_STATUS.at(statusCode);
}

/**
 * @brief レスポンス内容を平文に変換する
 *
 * @return std::string レスポンス内容
 */
std::string Response::toString() const
{
	std::string response;

	// ステータス行を平文に変換
	response += "HTTP/1.1 " + this->_status + " " +
				_statusMessage + "\r\n";

	// ヘッダーを平文に変換
	std::map<std::string, std::string>::const_iterator iter;
	for (iter = this->_headers.begin(); iter != this->_headers.end(); ++iter)
	{
		const std::pair<std::string, std::string> &pair = *iter;
		response += pair.first + ": " + pair.second + "\r\n";
	}
	// 終了行を追加
	response += "\r\n";
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
	setHeader(_headers, "date", dateNow);
}

void Response::setDateToCookie()
{
	const std::string &date = getHeader("date");
	setHeader(_headers, "set-cookie", "date=" + date);
}

/**
 * @brief Content-Lengthをセットする
 *
 * @detail Content-Lengthフィールドは、ボディの長さをバイト単位で表す。
 *
 */
void Response::setContentLength()
{
	setHeader(this->_headers, "content-length", to_string(this->_body.length()));
}

void Response::setServer()
{
	setHeader(_headers, "server", "Webserv 0.1");
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

void	Response::setFirstLine(const std::string &line)
{
	std::string::size_type	protocol_end = line.find(" ");
	std::string::size_type	status_end = line.find(" ", protocol_end + 1);
	_protocol = line.substr(0, protocol_end);
	_status = line.substr(protocol_end + 1, status_end - protocol_end - 1);
	_statusMessage = line.substr(status_end + 1);
}

bool Response::isValidLine(const std::string &line, const bool isFirstLine) const
{
	if (isFirstLine)
		return (isStartLine(line));
	else
		return (isHeaderField(line));
}

void	Response::makeRowString()
{
	_raw = toString();
}

const std::string	&Response::getStatusMessage() const { return _statusMessage; }