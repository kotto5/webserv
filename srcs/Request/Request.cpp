#include "Request.hpp"
#include "Error.hpp"
#include "Config.hpp"
#include "LocationContext.hpp"
#include "ServerContext.hpp"

// Constructors
Request::Request(const std::string &method, const std::string &uriAndQuery, const std::string &protocol,
				const std::map<std::string, std::string> &headers, const std::string &body)
	: _method(method), _uriAndQuery(uriAndQuery), _protocol(protocol), _headers(headers), _body(body)
{
	// クエリを取得
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

/**
 * @brief URIを実体パスに変換する
 *
 * @param uri
 * @return std::string
 */
std::string	Request::convertUritoPath(const std::string &uri)
{
	Config	*config = Config::getInstance();
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
		location = servercontext.getLocationContext(this->getUri());
	}
	catch (std::runtime_error &e)
	{
		std::cout << e.what() << std::endl;
		std::cout << "ERRRRRRRRRRRRRRRRRRRRRRRRRRR!!!!!!!!!" << std::endl;
		return ("404");
	}
	path = location.getDirective("path");
	if (location.getDirective("alias") != "")
		alias = location.getDirective("alias");
	else if (location.getDirective("root") != "")
		alias = location.getDirective("root");
	if (alias == "")
		return (uri);
	if (path == "")
		return (alias);
	return (alias + uri.substr(path.length()));
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int	Request::setaddr(int clientSocket)
{
	struct sockaddr_in	addr;
	socklen_t			addr_size = sizeof(struct sockaddr_in);
	int					result;

	memset(&addr, 0, addr_size);
	result = getsockname(clientSocket, (struct sockaddr*)&addr, &addr_size);
	if (result == -1)
	{
		Error::print_error("getsockname", Error::E_SYSCALL);
		return (-1);
	}
	_ip = inet_ntoa(addr.sin_addr);
	_port = ntohs(addr.sin_port);
	return (0);
}

// Not use
Request::Request() : _method(), _uri(), _headers(), _body() {}

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
	std::cout << "port: [" << _port << "]" << std::endl;
	std::cout << "content_length: [" << _content_length << "]" << std::endl;
	std::cout << "content_type: [" << _content_type << "]" << std::endl;
	std::cout << "cgi_script_name: [" << _cgi_script_name << "]" << std::endl;
	std::cout << "path_info: [" << _path_info << "]" << std::endl;
	std::cout << "actual_uri: [" << _actual_uri << "]" << std::endl;
}
