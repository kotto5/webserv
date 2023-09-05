#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <string>
#include "Socket.hpp"
#include "HttpMessage.hpp"
#include "LocationContext.hpp"
#include "ServerContext.hpp"

class Request : public HttpMessage
{
public:
	// Constructors
	Request(const std::string &method, const std::string &uriAndQuery, const std::string &protocol, std::map<std::string, std::string> headers, const std::string &body);
	Request(const Request &other);
	Request(const ClSocket *clientSocket);
	Request(const std::string &row);
	Request();

	// Destructor
	~Request();

	// Operators
	Request &operator=(const Request &rhs);

	// Methods
	static std::string	convertUriToPath(const std::string &uri, const std::string &port, const std::string &server_name);
	static std::string	convertUriToPath(const std::string &uri, const LocationContext &location);
	static std::string	convertUriToPath(const std::string &uri, const ServerContext &serverContext);
	static const LocationContext	&getLocationContext(const std::string &uri, const std::string &port, const std::string &server_name);
	static const ServerContext		&getServerContext(const std::string &port, const std::string &server_name);
	void				printAll() const;

	// Getters/Setters
	const std::string	&getMethod() const;
	const std::string	&getUri() const;
	const std::string	&getQuery() const;
	const std::string	&getActualUri() const;
	const std::string	&getServerName() const;
	const std::string	&getServerPort() const;
	const std::string	&getRemoteAddr() const;
	const std::string	&getRemoteHost() const;
	const std::string	&getContentType() const;
	const std::string	&getPathInfo() const;
	const std::string	&getIp() const;
	Request				&setAddr(const ClSocket *clientSocket);
	Request				&setInfo();

private:
	virtual	void		setFirstLine(const std::string &line);
	bool	isValidLine(const std::string &line, const bool isFirstLine) const;

	std::string _method;
	std::string _uriAndQuery;
	std::string _uri;
	std::string _query;
	std::string _actual_uri;
	std::string _ip;

	std::size_t _content_length;
	std::string	_content_type;
	std::string	_cgi_script_name;
	std::string	_path_info;
	std::string	_remote_addr;
	std::string	_remote_host;

	std::string	_server_name;
	std::string	_server_port;

};

#endif