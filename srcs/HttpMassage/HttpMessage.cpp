#include "HttpMessage.hpp"

std::string HttpMessage::_empty = "";

HttpMessage::HttpMessage()
    : _readPos(0), _isHeaderEnd(false), _isBodyEnd(false)
{
}

HttpMessage::~HttpMessage() {}

int	HttpMessage::parsing(const std::string &row)
{
	_readBuffer += row;
	std::cout << "row: [" << row << "]" << std::endl;

	std::string	line;
	std::string::size_type endPos;
	if (_isHeaderEnd == false)
	{
		while ((endPos = _readBuffer.find("\r\n", _readPos)) != _readPos)
		{
			if (endPos == std::string::npos) // no new line (incomplete)
				return (0);
			line = _readBuffer.substr(_readPos, endPos - _readPos);
			if (isValidLine(line, _readPos == 0) == false)
				return (1);
			if (_readPos == 0)
				setFirstLine(line);
			else
				setHeaderToLower(_headers, line, ": ");
			_readPos = endPos + 2; // Skip CRLF
		}
		_isHeaderEnd = true;
	}
	if (_isBodyEnd == true)
		return (0);
	setBody(row);
	// if (_isBodyEnd == true)
	// 	setinfo();
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

void	HttpMessage::setBody(const std::string &row)
{
	if (_headers["content-length"].empty() == false)
	{
		std::string::size_type	content_length = std::stoi(_headers["content-length"]);
		_body = row.substr(_readPos, content_length);
		if (_body.length() == content_length || _body.find("\r\n\r\n") != std::string::npos)
			_isBodyEnd = true;
		else
			_readPos += _body.length();
	}
	else if (_headers["transfer-encoding"] == "chunked")
	{
		if (_body.find("\r\n0\r\n\r\n") == std::string::npos)
			return ;
		// TODO: body = decode_chunked(body);
		_readPos += _body.length();
		_isBodyEnd = true;
	}
	else
		_isBodyEnd = true;
}

void	HttpMessage::setHeaderToLower(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword) {
	size_t pos = inputStr.find(keyword);

	if (pos != std::string::npos) {
		std::string part1 = inputStr.substr(0, pos);
		std::string part2 = inputStr.substr(pos + keyword.length());

		transform(part1.begin(), part1.end(), part1.begin(), ::tolower);
		m.insert(std::map<std::string, std::string>::value_type(part1, part2));
	}
}

bool	HttpMessage::isEnd() const
{
	return (_isHeaderEnd && _isBodyEnd);
}

const std::string   &HttpMessage::getRow() const
{
	return (_readBuffer);
}

const   std::string &HttpMessage::getBody() const {
    return (_body);
}

const std::string   &HttpMessage::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);
	if (it != this->_headers.end())
		return it->second;
	return _empty;
}

const std::string   &HttpMessage::getProtocol() const
{
    return (_protocol);
}
