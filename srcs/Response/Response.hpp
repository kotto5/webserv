#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <map>
#include <string>

class Response
{
public:
	// Constructors
	Response(const int status, std::map<std::string, std::string> headers, const std::string &body);
	Response(const Response &other);

	// Destructor
	~Response();

	// Operators
	Response &operator=(const Response &rhs);

	// Member functions
	std::string toString() const;

	// Getters/setters
	int getStatus() const;
	std::string getHeader(const std::string &key) const;
	std::string getBody() const;

private:
	int _status;
	std::map<std::string, std::string> _headers;
	std::string _body;

	const std::map<int, std::string> _statusMessages;

	std::string getStatusMessage(int statusCode) const;

	// Not use
	Response();
};

#endif