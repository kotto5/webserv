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
#include <cassert>
/**
 * @brief パスパターンが一致するLocationブロックを取得する
 *
 * @detail 記述順に走査し、最初に一致したブロックを返す
 * @param path 検索するパスパターン
 * @return const LocationContext& 一致したLocationContext
 */
const LocationContext& ServerContext::getLocationContext(const std::string& path) const
{
	std::vector<LocationContext> location = getLocations();
	std::vector<LocationContext>::iterator it;
	for (it = location.begin(); it != location.end(); ++it)
	{
		if (it->getPath() == path)
			return *it;
	};
	throw std::runtime_error("Path not found");
}
