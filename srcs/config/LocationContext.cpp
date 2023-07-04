#include "LocationContext.hpp"

LocationContext::LocationContext():
	_path(),
	_alias(),
	_index(),
	_error_page()
{
}

LocationContext::~LocationContext()
{
}

LocationContext::LocationContext(const LocationContext &other):
	_path(other._path),
	_alias(other._alias),
	_index(other._index),
	_error_page(other._error_page)
{
}

LocationContext &LocationContext::operator=(const LocationContext &rhs)
{
	if (this != &rhs)
	{
		_path = rhs._path;
		_alias = rhs._alias;
		_index = rhs._index;
		_error_page = rhs._error_page;
	}
	return *this;
}

void LocationContext::setPath(const std::string& path)
{
	_path = path;
}

void LocationContext::setAlias(const std::string& alias)
{
	_alias = alias;
}

void LocationContext::setIndex(const std::string& index)
{
	_index = index;
}

void LocationContext::addErrorPage(const int status_code, const std::string& error_page)
{
	_error_page.insert(std::make_pair(status_code, error_page));
}

const std::string& LocationContext::getPath() const
{
	return _path;
}

const std::string& LocationContext::getAlias() const
{
	return _alias;
}

const std::string& LocationContext::getIndex() const
{
	return _index;
}

const std::map<int, std::string>& LocationContext::getErrorPage() const
{
	return _error_page;
}