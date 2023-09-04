#include "HttpMessage.hpp"
#include "HTTPContext.hpp"
#include "Config.hpp"
#include <algorithm>
#include <cstring>


std::string HttpMessage::_empty = "";

HttpMessage::HttpMessage()
    : _raw(), _protocol(), _headers(), _body(), _isHeaderEnd(false),
	_isBodyEnd(false), _readPos(0), _sendPos(0), _sendBuffer(NULL), _doesSendEnd(false)
{
	_tooBigError = false;
	_contentLength = 0;
	_isChunked = false;
	_isBadFormat = false;
}

HttpMessage::HttpMessage(const std::string &_raw)
	: _raw(_raw), _protocol(), _headers(), _body(), _isHeaderEnd(false),
	_isBodyEnd(false), _readPos(0), _sendPos(0), _sendBuffer(NULL), _doesSendEnd(false)
{
	_tooBigError = false;
	_contentLength = 0;
}

HttpMessage::~HttpMessage() {
	if (_sendBuffer != NULL)
		delete[] _sendBuffer;
}

/**
 * @brief リクエスト・レスポンスの解析
 *
 * @param raw リクエスト・レスポンスの生データ
 * @param maxSize 最大サイズ
 * @return int
 */
int	HttpMessage::parsing(const std::string &raw, const std::size_t maxSize)
{
	_raw += raw;
	// 最大サイズを超えたらエラー
	if (maxSize != 0 && _raw.length() > maxSize)
	{
		_tooBigError = true;
		return (1);
	}
	#ifdef TEST
		std::cout << "--------" << std::endl;
		std::cout << raw << std::endl;
		std::cout << maxSize <<std::endl;
		std::cout << "parsing raw length: [" << _raw.length() << "]" << std::endl;
	#endif

	std::string	line;
	std::string::size_type endPos;

	// ヘッダーの解析
	if (_isHeaderEnd == false)
	{
		while ((endPos = _raw.find("\r\n", _readPos)) != _readPos)
		{
			if (endPos == std::string::npos) // no new line (incomplete)
				return (0);
			line = _raw.substr(_readPos, endPos - _readPos);
			if (isValidLine(line, _readPos == 0) == false)
			{
				_isBadFormat = true;
				return (1);
			}
			if (_readPos == 0)
				setFirstLine(line);
			else
				HttpMessage::setHeaderFromLine(_headers, line, ": ");
			_readPos = endPos + 2; // Skip CRLF
		}
		_readPos = endPos + 2; // Skip CRLF
		_isHeaderEnd = true;
		if (getHeader("content-length").empty() == false)
			_contentLength = std::stoi(_headers["content-length"]);
		if (getHeader("transfer-encoding") == "chunked")
			_isChunked = true;
		setBody(_raw.substr(_readPos));
	}
	else if (_isBodyEnd == false)
		setBody(raw); // 引数そのまま渡す
	return (0);
}

bool HttpMessage::isValidLine(const std::string &line, const bool isFirstLine) const
{
	if (isFirstLine)
	{
		long sp_count = std::count(line.begin(), line.end(), ' ');
		if (sp_count != 2)
			return (false);
		std::string::size_type	sp1 = line.find(" ");
		std::string::size_type	sp2 = line.find(" ", sp1 + 1);
		if (sp1 == sp2 + 1 || sp2 == static_cast<std::string::size_type>(line.end() - line.begin() - 1))
			return (false);
	}
	else
	{
		std::string::size_type	colon = line.find(": ");
		if (colon == std::string::npos || colon == 0 || colon == line.length() - 2)
			return (false);
	}
	return (true);
}

/**
 * @brief リクエスト／レスポンスボディをセットする
 *
 * @param addBody
 */
void	HttpMessage::setBody(const std::string &addBody)
{
	// lengthが指定されている場合
	if (_contentLength)
	{
		if (_body.empty())
			_body.reserve(_contentLength);
		_body += addBody;
		if (_body.length() >= _contentLength)
		{
			_body = _body.substr(0, _contentLength);
			_isBodyEnd = true;
		}
		else
			_readPos += addBody.length();
	} // chunkedが指定されている場合
	else if (_isChunked)
	{
		_body += addBody;
		_readPos += _body.length();
		if (_body.find("\r\n0\r\n\r\n") != std::string::npos)
		{
			_isBodyEnd = true;
			decodeChunked(_body);
		}
	}
	////// クソコード　要検討
	else if (addBody.length() > 0) // cgi の時の想定。。。 あんまダメかも client の時も影響受けるから。
	{
		_body += addBody;
		_readPos += addBody.length();
		_isBodyEnd = false;
	}
	else
		_isBodyEnd = true;
}

/**
 * @brief チャンク化されたデータを復号する
 *
 * @detail この関数は副作用を持つ
 * @param body チャンク化された文字列
 * @return void
 */
void	HttpMessage::decodeChunked(std::string &body)
{
    size_t         readPos = 0;
    std::string    dechunk = "";
    unsigned long chunkSize;

    chunkSize = 1;
    while (chunkSize)
    {
		chunkSize = std::stoul(body.substr(readPos), NULL, 16);
        readPos += chunkSize / 16 + 1;
        if (body.at(readPos) != '\r' || body.at(readPos + 1) != '\n')
		{
			_isBadFormat = true;
			return ;
		}
        readPos += 2;
        dechunk += body.substr(readPos, chunkSize);
        readPos += chunkSize;
        if (body.at(readPos) != '\r' || body.at(readPos + 1) != '\n')
        {
			_isBadFormat = true;
			return ;
		}
        readPos += 2;
    }
    body = dechunk;
}

std::string	HttpMessage::makeHeaderKeyLower(std::string key)
{
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	return (key);
}

bool	HttpMessage::isCompleted() const
{
	return (_isHeaderEnd && _isBodyEnd);
}

void	HttpMessage::addSendPos(std::size_t pos)
{
	_sendPos += pos;
	if (_sendPos == _sendBufferSize)
	{
		_doesSendEnd = true;
		delete[] _sendBuffer;
		_sendBuffer = NULL;
	}
}

bool	HttpMessage::doesSendEnd() const
{
	return (_doesSendEnd);
}
#define TEST

const uint8_t	*HttpMessage::getSendBuffer()
{
	if (_doesSendEnd == true)
		return (NULL);
	if (_sendBuffer == NULL)
	{
		#ifdef TEST
			std::cout << "getSendBuffer: _raw is[" << _raw << "]" << std::endl;
		#endif
		_sendBufferSize = _raw.length();
		try {
			_sendBuffer = new  uint8_t[_sendBufferSize];
		} catch (std::exception &e) {
			return (NULL);
		}
		std::memcpy((void *)_sendBuffer, (void *)_raw.c_str(), _raw.length());
	}
	return (_sendBuffer);
}

std::size_t	HttpMessage::getContentLength() const {
	return (_contentLength);
}

std::size_t	HttpMessage::getContentLengthRemain() const {
	return (_sendBufferSize - _sendPos);
}

void	HttpMessage::printHeader() const
{
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		std::cout << "\t[" << it->first << "]: [" << it->second << "]" << std::endl;
}

bool	HttpMessage::getIsBadFormat() const
{
	return (_isBadFormat);
}

bool	HttpMessage::isInvalid() const
{
	return (_isBadFormat || _tooBigError);
}

bool	HttpMessage::isTooBigError() const
{
	return (_tooBigError);
}

const   std::string &HttpMessage::getBody() const {
    return (_body);
}

const std::string   &HttpMessage::getHeader(std::string key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(makeHeaderKeyLower(key));
	if (it != this->_headers.end())
		return it->second;
	return _empty;
}

const std::string   &HttpMessage::getProtocol() const
{
    return (_protocol);
}

const std::string   &HttpMessage::getRaw() const
{
	return (_raw);
}

void	HttpMessage::setHeader(std::map<std::string, std::string>& m, std::string first, std::string second)
{
	m[makeHeaderKeyLower(first)] = second;
}

void	HttpMessage::setHeaderFromLine(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword) {
	size_t pos = inputStr.find(keyword);
	if (pos == std::string::npos)
		return ;

	std::string first = inputStr.substr(0, pos);
	std::string second = inputStr.substr(pos + keyword.length());
	setHeader(m, first, second);
}

void	HttpMessage::setContentLength() {
	const std::string &contentLengthValue = getHeader("content-length");
	_contentLength = contentLengthValue.empty() ? 0 : std::stoi(contentLengthValue);
}


void	HttpMessage::addHeader(std::string key, std::string value)
{
	setHeader(_headers, key, value);
}

void	HttpMessage::setBodyEnd(bool isBodyEnd) { _isBodyEnd = isBodyEnd; }

bool    HttpMessage::isStartLine(const std::string &line) const
{
    long sp_count = std::count(line.begin(), line.end(), ' ');
    if (sp_count != 2)
        return (false);
    std::string::size_type	sp1 = line.find(" ");
    std::string::size_type	sp2 = line.find(" ", sp1 + 1);
    if (sp1 == sp2 + 1 || sp2 == static_cast<std::string::size_type>(line.end() - line.begin() - 1))
        return (false);
    return (true);
}

bool    HttpMessage::isHeaderField(const std::string &line) const
{
    std::string::size_type	colon = line.find(": ");
    if (colon == std::string::npos || colon == 0 || colon == line.length() - 2)
        return (false);
    return (true);
}

const std::map<std::string , std::string> &HttpMessage::getHeaders() const { return (_headers); }