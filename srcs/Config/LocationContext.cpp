#include "LocationContext.hpp"
#include "ConfigException.hpp"
#include <stdexcept>

LocationContext::LocationContext():
	//_error_page(),
	_directives()
{
}

LocationContext::LocationContext(const LocationContext &other)
{
	*this = other;
}

LocationContext::~LocationContext()
{
}

LocationContext &LocationContext::operator=(const LocationContext& other)
{
	if (this != &other)
	{
		_directives = other._directives;
	}
	return *this;
}

void LocationContext::addDirective(const std::string& directive, const std::string& value,
	const std::string& filepath, int line_number)
{
	// check if directive is not duplicated
	if (_directives.find(directive) != _directives.end())
		throw ConfigException(ErrorCode::CONF_DUPLICATE_DIRECTIVE, directive, filepath, line_number);

	_directives.insert(std::make_pair(directive, value));
}

std::string	LocationContext::getDirective(const std::string& directive) const
{
	std::map<std::string, std::string>::const_iterator it = _directives.find(directive);
	if (it == _directives.end())
		return "";
	return it->second;
}
