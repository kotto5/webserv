#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <string>

class Request
{
public:
	// Constructors
	Request(const std::string &method, const std::string &uri,
			const std::map<std::string, std::string> &headers, const std::string &body);
	Request(const Request &other);

	// Destructor
	~Request();

	// Operators
	Request &operator=(const Request &rhs);

	// Getters/Setters
	std::string getMethod() const;
	std::string getUri() const;
	std::string getHeader(const std::string &key) const;
	std::string getBody() const;

private:
	std::string _method;
	std::string _uri;
	std::map<std::string, std::string> _headers;
	std::string _body;

	// Not use
	Request();
};

#endif