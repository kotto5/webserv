#include "LocationContext.hpp"

LocationContext::LocationContext():
	_alias(),
	_index(),
	_error_page()
{
}

LocationContext::~LocationContext()
{
}

void LocationContext::setAlias(const std::string &alias)
{
	_alias = alias;
}

void LocationContext::setIndex(const std::string &index)
{
	_index = index;
}

void LocationContext::addErrorPage(const int status_code, const std::string &error_page)
{
	_error_page.insert(std::make_pair(status_code, error_page));
}

const std::string &LocationContext::getAlias() const
{
	return _alias;
}

const std::string &LocationContext::getIndex() const
{
	return _index;
}

const std::map<int, std::string> &LocationContext::getErrorPage() const
{
	return _error_page;
}