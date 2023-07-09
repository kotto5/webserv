#include "LocationContext.hpp"
#include <stdexcept>

LocationContext::LocationContext():
	//_error_page(),
	_directives()
{
}

LocationContext::~LocationContext()
{
}

// void LocationContext::addErrorPage(const std::string& status_code, const std::string& error_page)
// {
// 	_error_page.insert(std::make_pair(status_code, error_page));
// }

void LocationContext::addDirective(const std::string& directive, const std::string& value)
{
	_directives.insert(std::make_pair(directive, value));
}

// const std::string& LocationContext::getErrorPage(const std::string& status_code) const
// {
// 	std::map<std::string, std::string>::const_iterator it = _error_page.find(status_code);
// 	if (it == _error_page.end())
// 		throw std::runtime_error("error_page not found");
// 	return it->second;
// }

const std::string& LocationContext::getDirective(const std::string& directive) const
{
	std::map<std::string, std::string>::const_iterator it = _directives.find(directive);
	if (it == _directives.end())
		throw std::runtime_error("directive not found");
	return it->second;
}
