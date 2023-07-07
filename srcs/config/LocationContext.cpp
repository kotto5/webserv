#include "LocationContext.hpp"

LocationContext::LocationContext():
	_error_page(),
	_directives()
{
}

LocationContext::~LocationContext()
{
}

void LocationContext::addErrorPage(const std::string& status_code, const std::string& error_page)
{
	_error_page.insert(std::make_pair(status_code, error_page));
}

void LocationContext::addDirective(const std::string& directive, const std::string& value)
{
	_directives.insert(std::make_pair(directive, value));
}

const std::map<std::string, std::string>& LocationContext::getErrorPage() const
{
	return _error_page;
}
const std::map<std::string, std::string>& LocationContext::getDirective() const
{
	return _directives;
}
