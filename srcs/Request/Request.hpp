#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>

class Request
{
	public:
		// Constructors
		Request();
		Request(const Request &other);

		// Destructor
		~Request();

		// Operators
		Request & operator=(const Request &rhs);

	private:
		std::string _method;
		std::string _uri;
		std::map <std::string, std::string> _headers;
		std::string _body;
};

#endif