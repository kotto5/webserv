#include "Request.hpp"
#include "ErrorCode.hpp"
#include "Config.hpp"
#include "ConfigException.hpp"
#include "LocationContext.hpp"
#include "ServerContext.hpp"
#include "Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "utils.hpp"
#include <algorithm>

// Constructors
Request::Request(const std::string &method, const std::string &uriAndQuery, const std::string &protocol,
				const std::map<std::string, std::string> &headers, const std::string &body)
	: _method(method), _uriAndQuery(uriAndQuery), _protocol(protocol), _headers(headers), _body(body)
{
	setinfo();
}

void	Request::setinfo()
{
	std::string::size_type pos = this->_uriAndQuery.find("?");
	this->_uri = pos == std::string::npos ? this->_uriAndQuery : this->_uriAndQuery.substr(0, pos);
	this->_query = pos == std::string::npos ? "" : this->_uriAndQuery.substr(pos + 1);

	// ヘッダーのContent-Lengthを取得
	this->_content_length = this->_body.length();

	// ヘッダーのContent-Typeを取得
	this->_content_type = this->getHeader("Content-Type");

	// aliasとrootを考慮したuriを取得
	this->_actual_uri = convertUritoPath(this->_uri);
	// CGIに用いるscript_nameとpath_infoを取得
	// this->_cgi_script_name = this->get
	this->_path_info = this->_uri.find(_cgi_script_name) == std::string::npos ?
	"" : this->_uri.substr(this->_uri.find(_cgi_script_name) + _cgi_script_name.length());
}

Request::Request():_isHeaderEnd(false), _isBodyEnd(false){}

static std::string	getAliasOrRootDirective(LocationContext &Location)
{
	std::string ret = Location.getDirective("alias");
	if (ret.empty())
		ret = Location.getDirective("root");
	if (ret.empty())
		return ("");
	if (ret[ret.length() - 1] != '/')
		ret += '/';
	return (ret);
}

/**
 * @brief URIを実体パスに変換する
 *
 * @param uri
 * @return std::string
 */
std::string	Request::convertUritoPath(const std::string &uri)
{
	Config	*config = Config::instance();
	HTTPContext	httpcontext;
	ServerContext	servercontext;
	LocationContext	location;

	std::string	ret = uri;
	std::string alias = "";
	std::string path = "";

	try
	{
		httpcontext = config->getHTTPBlock();
		servercontext = httpcontext.getServerContext("80", this->getHeader("host"));
		location = servercontext.getLocationContext(ret);
	}
	catch (const std::runtime_error &e)
	{
		std::cout << e.what() << std::endl;
		return ("404");
	}
	path = location.getDirective("path");
	// URI < PATH
	if (ret.length() < path.length()) // path が /path/ に対し uri が /path だった場合の対応
		ret = path;
	alias = getAliasOrRootDirective(location);
	if (alias == "")
		return (ret);
	if (path == ret)
		return (alias + location.getDirective("index"));
	return (alias + ret.substr(path.length()));
}

Request::Request(const Request &other)
{
	*this = other;
}

// Destructor
Request::~Request() {}

// Operators
Request &Request::operator=(const Request &rhs)
{
	if (this != &rhs)
	{
		this->_method = rhs._method;
		this->_uriAndQuery = rhs._uriAndQuery;
		this->_protocol = rhs._protocol;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
	}
	return *this;
}

// Getters/Setters
std::string Request::getMethod() const
{
	return this->_method;
}

std::string Request::getUri() const
{
	return this->_uri;
}

std::string Request::getProtocol() const
{
	return this->_protocol;
}

std::string Request::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);
	if (it != this->_headers.end())
		return it->second;
	return "";
}

std::string Request::getBody() const
{
	return this->_body;
}

std::string Request::getActualUri() const
{
	return this->_actual_uri;
}

int	Request::setaddr(ClSocket *clientSocket)
{
	struct sockaddr_in	addr;
	socklen_t			addr_size = sizeof(struct sockaddr_in);

	addr = clientSocket->getLocaladdr();
	addr_size = clientSocket->getLocallen();
	int	port = ntohs(addr.sin_port);

	_server_name = inet_ntoa(addr.sin_addr);
	_server_port = std::to_string(port);

	addr = clientSocket->getRemoteaddr();
	addr_size = clientSocket->getRemotelen();

	_remote_addr = inet_ntoa(addr.sin_addr);
	_remote_host = _remote_addr;

	return (0);
}

void	Request::print_all(void) const
{
	std::cout << "method: [" << _method << "]" << std::endl;
	std::cout << "uri: [" << _uri << "]" << std::endl;
	std::cout << "protocol: [" << _protocol << "]" << std::endl;
	std::cout << "headers: [" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		std::cout << "\t" << it->first << ": " << it->second << std::endl;
	std::cout << "]" << std::endl;
	std::cout << "body: [" << _body << "]" << std::endl;
	std::cout << "ip: [" << _ip << "]" << std::endl;
	std::cout << "content_length: [" << _content_length << "]" << std::endl;
	std::cout << "content_type: [" << _content_type << "]" << std::endl;
	std::cout << "cgi_script_name: [" << _cgi_script_name << "]" << std::endl;
	std::cout << "path_info: [" << _path_info << "]" << std::endl;
	std::cout << "actual_uri: [" << _actual_uri << "]" << std::endl;
	std::cout << "remote_addr: [" << _remote_addr << "]" << std::endl;
	std::cout << "remote_host: [" << _remote_host << "]" << std::endl;
	std::cout << "server_name: [" << _server_name << "]" << std::endl;
	std::cout << "server_port: [" << _server_port << "]" << std::endl;
}

void	Request::addHeaderToLower(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword) {
	size_t pos = inputStr.find(keyword);

	if (pos != std::string::npos) {
		std::string part1 = inputStr.substr(0, pos);
		std::string part2 = inputStr.substr(pos + keyword.length());

		transform(part1.begin(), part1.end(), part1.begin(), ::tolower);
		m.insert(std::map<std::string, std::string>::value_type(part1, part2));
	}
}

void	Request::setRequestLine(const std::string &line, std::string &method, std::string &uri, std::string &protocol)
{
	std::string::size_type	method_end = line.find(" ");
	std::string::size_type	uri_end = line.find(" ", method_end + 1);
	method = line.substr(0, method_end);
	uri = line.substr(method_end + 1, uri_end - method_end - 1);
	protocol = line.substr(uri_end + 1);
}

static bool	isValidLine(const std::string &line, const bool isRequestLine)
{
	if (isRequestLine)
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

bool	setBody(std::string &body, const std::string &row, const std::string::size_type startPos, const std::string::size_type content_length)
{
	// std::cout << "content_length: " << content_length << std::endl;
	body += row.substr(startPos + 2, content_length);
	return (true);
}

Request	*Request::parse(const std::string &row)
{
	std::string method;
	std::string uri;
	std::string protocol;
	std::map<std::string, std::string> headers;
	std::string body;

	std::string::size_type startPos = 0;
	std::string::size_type endPos;
	std::string	line;
	while ((endPos = row.find("\r\n", startPos)) != std::string::npos)
	{
		if (endPos == startPos) // empty line
			break;
		line = row.substr(startPos, endPos - startPos);		
		if (isValidLine(line, startPos == 0) == false)
			return (NULL);
		if (startPos == 0)
			setRequestLine(line, method, uri, protocol);
		else
			addHeaderToLower(headers, line, ": ");
		startPos = endPos + 2; // Skip CRLF
	}
	std::string content_length = headers["content-length"];
	if (content_length.empty() == false)
		setBody(body, row, startPos, std::stoi(content_length));
	if (headers.find("Transfer-Encoding") != headers.end() && headers["Transfer-Encoding"] == "chunked")
	{
		std::string::size_type	end_of_body = row.find("\r\n0\r\n\r\n");
		body = row.substr(startPos, end_of_body - startPos);
		// TODO: body = decode_chunked(body);
	}
	return (new Request(method, uri, protocol, headers, body));
}

int	Request::parsing(const std::string &row)
{
	_readBuffer += row;

	std::string	line;
	std::string::size_type endPos;
	if (_isHeaderEnd == false)
	{
		while ((endPos = row.find("\r\n", _readPos)) != _readPos)
		{
			if (endPos == std::string::npos) // no new line (incomplete)
				return (0);
			line = row.substr(_readPos, endPos - _readPos);
			if (isValidLine(line, _readPos == 0) == false)
				return (0);
			if (_readPos == 0)
				setRequestLine(line, _method, _uri, _protocol);
			else
				addHeaderToLower(_headers, line, ": ");
			_readPos = endPos + 2; // Skip CRLF
		}
		_isHeaderEnd = true;
	}
	if (_isBodyEnd == false)
		return (0);
	_body += row.substr(_readPos);
	if (_headers["content-length"].empty() == false)
	{
		if (_body.find("\r\n\r\n") == std::string::npos)
			return (0);
		std::string::size_type	content_length = std::stoi(_headers["content-length"]); 
		if (_body.length() > content_length)
			_body.erase(content_length);
		_isBodyEnd = true;
	}
	else if (_headers["transfer-encoding"] == "chunked")
	{
		if (_body.find("\r\n0\r\n\r\n") == std::string::npos)
			return (0);
		// TODO: body = decode_chunked(body);
		_isBodyEnd = true;
	}
	else
		_isBodyEnd = true;
	if (_isBodyEnd == true)
	{
		_readBuffer.clear();
		setinfo();
	}
	return (0);
}

bool	Request::isEnd() const {
	return (_isHeaderEnd && _isBodyEnd);
}

std::string	Request::getRowRequest() const
{
	return (_readBuffer);
}