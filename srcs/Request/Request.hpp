#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <string>

class Request
{
public:
	// Constructors
	Request(const std::string &method, const std::string &uriAndQuery, const std::string &protocol,
			const std::map<std::string, std::string> &headers, const std::string &body);
	Request(const std::string &row_request);
	Request(const Request &other);

	// Destructor
	~Request();

	// Operators
	Request &operator=(const Request &rhs);

	// Getters/Setters
	std::string getMethod() const;
	std::string getUri() const;
	std::string	getProtocol() const;
	std::string getHeader(const std::string &key) const;
	std::string getBody() const;
	std::string	convertUritoPath(const std::string &uri);
	void		print_all() const;
	int			seturi();

	int	setaddr(int clientSocket);

private:
	std::string _method;
	std::string _uriAndQuery;
	std::string _uri;
	std::string _query;
	std::string _protocol;
	std::map<std::string, std::string> _headers;
	std::string _body;

	std::string _acutual_uri;

	std::string _ip;
	int			_port;
	std::size_t _content_length;
	std::string	_content_type;
	std::string	_cgi_script_name;
	std::string	_path_info;

	// Not use
	Request();
};

#endif