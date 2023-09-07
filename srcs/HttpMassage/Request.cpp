#include "Request.hpp"
#include "ErrorCode.hpp"
#include "Config.hpp"
#include "ConfigException.hpp"
#include "Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "utils.hpp"
#include <algorithm>
#include <sstream>

Request::Request(const std::string &method, const std::string &uriAndQuery, const std::string &protocol, std::map<std::string, std::string> headers, const std::string &body)
{
	this->_method = method;
	this->_uriAndQuery = uriAndQuery;
	this->_protocol = protocol;
	this->_headers = headers;
	this->_body = body;
}

Request::Request(const std::string &row): HttpMessage(row) {}


Request::Request(const ClSocket *clientSocket)
{
	this->setAddr(clientSocket);
}

Request::Request(){}

static std::string	getAliasOrRootDirective(const LocationContext &Location)
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

const LocationContext	&Request::getLocationContext(const std::string &uri, const std::string &port, const std::string &server_name)
{
	return Config::instance()->getHTTPBlock()
		.getServerContext(port, server_name)
		.getLocationContext(uri);
}

const	ServerContext	&Request::getServerContext(const std::string &port, const std::string &server_name)
{
	return Config::instance()->getHTTPBlock()
		.getServerContext(port, server_name);
}

const	ServerContext	&Request::getServerContext(int port)
{
	return Config::instance()->getHTTPBlock()
		.getServerContext(to_string(port), "");
}

/**
 * @brief URIを実体パスに変換する
 *
 * @param uri
 * @return std::string
 */
std::string	Request::convertUriToPath(const std::string &uri, const std::string &port, const std::string &server_name)
{
	try
	{
		const ServerContext &svc = getServerContext(port, server_name);
		return (convertUriToPath(uri, svc));
	}
	catch (const std::runtime_error &e)
	{
		std::cout << e.what() << std::endl;
		return ("404");
	}
}

std::string	Request::convertUriToPath(const std::string &uri, const ServerContext &serverContext)
{
	std::string	ret = uri;
	std::string alias = "";
	std::string path = "";
	const LocationContext &location = serverContext.getLocationContext(uri);

	path = location.getDirective("path");
	if (ret.length() < path.length()) // path が /path/ に対し uri が /path だった場合の対応
		ret = path;
	alias = getAliasOrRootDirective(location);
	if (alias == "")
		return (ret);
	if (path == ret)
		return (alias + location.getDirective("index"));
	return (alias + ret.substr(path.length()));
}

Request::Request(const Request &other): HttpMessage()
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

void	Request::setFirstLine(const std::string &line)
{
	std::string::size_type	method_end = line.find(" ");
	std::string::size_type	uri_end = line.find(" ", method_end + 1);
	_method = line.substr(0, method_end);
	_uriAndQuery = percentDecode(line.substr(method_end + 1, uri_end - method_end - 1));
	_protocol = line.substr(uri_end + 1);
}

bool Request::isValidLine(const std::string &line, const bool isFirstLine) const
{
	if (isFirstLine)
		return (isStartLine(line));
	else
		return (isHeaderField(line));
}

void	Request::printAll(void) const
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
	std::cout << "query: [" << _query << "]" << std::endl;
	std::cout << "remote_addr: [" << _remote_addr << "]" << std::endl;
	std::cout << "remote_host: [" << _remote_host << "]" << std::endl;
	std::cout << "server_name: [" << _server_name << "]" << std::endl;
	std::cout << "server_port: [" << _server_port << "]" << std::endl;
}

// Getters/Setters
const std::string	&Request::getMethod() const {
	return this->_method;
}

const std::string	&Request::getUri() const {
	return this->_uri;
}

const std::string	&Request::getActualUri() const {
	return this->_actual_uri;
}

const std::string &Request::getServerName() const
{
	return this->_server_name;
}

const std::string &Request::getServerPort() const
{
	return this->_server_port;
}

const std::string &Request::getRemoteAddr() const
{
	return this->_remote_addr;
}

const std::string &Request::getRemoteHost() const
{
	return this->_remote_host;
}

const std::string &Request::getIp() const
{
	return this->_ip;
}

/**
 * @brief 接続先情報をセットする
 *
 * @param clientSocket
 * @return int
 */
Request &Request::setAddr(const ClSocket *clientSocket)
{
	struct sockaddr_in	addr;
	// socklen_t			addr_size = sizeof(struct sockaddr_in);

	addr = clientSocket->getLocalAddr();
	// addr_size = clientSocket->getLocalLen();
	int	port = ntohs(addr.sin_port);

	_server_name = inet_ntoa(addr.sin_addr);
	_server_port = to_string(port);

	addr = clientSocket->getRemoteAddr();
	// addr_size = clientSocket->getRemoteLen();

	_remote_addr = inet_ntoa(addr.sin_addr);
	_remote_host = _remote_addr;
	return *this;
}

/**
 * @brief リクエストのメタ情報をセットする
 *
 */
Request	&Request::setInfo()
{
	std::string::size_type pos = this->_uriAndQuery.find("?");

	this->_uri = pos == std::string::npos
		? this->_uriAndQuery
		: this->_uriAndQuery.substr(0, pos);
	this->_query = pos == std::string::npos
		? ""
		: this->_uriAndQuery.substr(pos + 1);

	// ヘッダーのContent-Lengthを取得
	this->_content_length = this->_body.length();

	// ヘッダーのContent-Typeを取得
	this->_content_type = this->getHeader("Content-Type");

	// aliasとrootを考慮したuriを取得
	this->_actual_uri = convertUriToPath(this->_uri, getServerPort(), getHeader("host"));
	// CGIに用いるscript_nameとpath_infoを取得
	// this->_cgi_script_name = this->get
	this->_path_info = this->_uri.find(_cgi_script_name) == std::string::npos
		? ""
		: this->_uri.substr(this->_uri.find(_cgi_script_name) + _cgi_script_name.length());
	return *this;
}

const std::string	&Request::getContentType() const {
	return this->_content_type;
}

const std::string	&Request::getQuery() const {
	return this->_query;
}

const std::string	&Request::getPathInfo() const {
	return this->_path_info;
}

const LocationContext &Request::getLocationContext() const
{
	return Config::instance()->getHTTPBlock()
		.getServerContext(_server_port, _server_name)
		.getLocationContext(_uri);
}

bool	Request::isCompleted() const {
	return (_isHeaderEnd && _isBodyEnd && getHeader("host") != "");
}