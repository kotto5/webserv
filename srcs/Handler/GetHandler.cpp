#include "GetHandler.hpp"
#include <fstream>
#include <sstream>

// Constructors
GetHandler::GetHandler() {}

GetHandler::GetHandler(const GetHandler &other)
{
	(void)other;
}

// Destructor
GetHandler::~GetHandler() {}

// Operators
GetHandler &GetHandler::operator=(const GetHandler &rhs)
{
	if (this != &rhs)
	{
		(void)rhs;
	}
	return *this;
}

Response GetHandler::handleRequest(const Request &request) const
{
	(void)request;
	std::ifstream htmlFile("docs/index.html");

	if (!htmlFile.is_open())
	{
		throw std::runtime_error("Could not open file");
	}
	std::stringstream buffer;
	buffer << htmlFile.rdbuf();
	htmlFile.close();
	std::string body = buffer.str();

	std::cout << body << std::endl;
	std::map<std::string, std::string> headers;
	return Response(200, headers, body);
}
