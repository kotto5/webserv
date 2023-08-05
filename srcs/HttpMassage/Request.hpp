#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <string>
#include "Socket.hpp"
#include "HttpMessage.hpp"

class Request : public HttpMessage
{
public:
	// Constructors
	Request(const std::string &method, const std::string &uriAndQuery, const std::string &protocol, std::map<std::string, std::string> headers, const std::string &body);
	Request(const Request &other);
	Request();

	// Destructor
	~Request();

	// Operators
	Request &operator=(const Request &rhs);

	// Getters/Setters
	const std::string	&getMethod() const;
	const std::string	&getUri() const;
	const std::string	&getActualUri() const;

	std::string	convertUritoPath(const std::string &uri);
	void		print_all() const;
	int			seturi();
	int			setaddr(ClSocket *clientSocket);
	void		setinfo();
	bool						isBadRequest() const;

private:
	virtual	void			setFirstLine(const std::string &line);

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