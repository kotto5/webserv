#include "ServerContext.hpp"

ServerContext::ServerContext():
	_listen(),
	_server_name()
	// _index(),
	// _error_page()
{
}

ServerContext::~ServerContext()
{
}

void ServerContext::setListen(int listen)
{
	_listen = listen;
}

void ServerContext::setServerName(const std::string& server_name)
{
	_server_name = server_name;
}

int ServerContext::getListen() const
{
	return _listen;
}

const std::string& ServerContext::getServerName() const
{
	return _server_name;
}

void ServerContext::addLocationBlock(const LocationContext& location)
{
	_locations.push_back(location);
}

const std::vector<LocationContext>& ServerContext::getLocations() const
{
	return _locations;
}
