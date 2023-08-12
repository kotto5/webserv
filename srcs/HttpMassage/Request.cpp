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

Request::Request(const std::string &method, const std::string &uriAndQuery, const std::string &protocol, std::map<std::string, std::string> headers, const std::string &body)
{
	this->_method = method;
	this->_uriAndQuery = uriAndQuery;
	this->_protocol = protocol;
	this->_headers = headers;
	this->_body = body;
	this->setinfo();
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

Request::Request(){}

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
	HTTPContext		httpcontext;
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
const std::string	&Request::getMethod() const {
	return this->_method;
}

const std::string	&Request::getUri() const {
	return this->_uri;
}

const std::string	&Request::getActualUri() const {
	return this->_actual_uri;
}

int	Request::setaddr(ClSocket *clientSocket)
{
	struct sockaddr_in	addr;
	// socklen_t			addr_size = sizeof(struct sockaddr_in);

	addr = clientSocket->getLocaladdr();
	// addr_size = clientSocket->getLocallen();
	int	port = ntohs(addr.sin_port);

	_server_name = inet_ntoa(addr.sin_addr);
	_server_port = std::to_string(port);

	addr = clientSocket->getRemoteaddr();
	// addr_size = clientSocket->getRemotelen();

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

void	Request::setFirstLine(const std::string &line)
{
	std::string::size_type	method_end = line.find(" ");
	std::string::size_type	uri_end = line.find(" ", method_end + 1);
	_method = line.substr(0, method_end);
	_uriAndQuery = percentDecode(line.substr(method_end + 1, uri_end - method_end - 1));
	_protocol = line.substr(uri_end + 1);
}

bool	Request::isBadRequest() const
{
	return (!isEnd());
}
