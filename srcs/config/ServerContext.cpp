#include "ServerContext.hpp"

ServerContext::ServerContext():
	_listen(),
	_server_name()
	// _index(),
	// _error_page()
{
}

ServerContext::ServerContext(const ServerContext &other):
	_listen(other._listen),
	_server_name(other._server_name),
	_locations(other._locations)
	// _index(other._index),
	// _error_page(other._error_page)
{
}

ServerContext &ServerContext::operator=(const ServerContext &rhs)
{
	if (this != &rhs)
	{
		_listen = rhs._listen;
		_server_name = rhs._server_name;
		_locations = rhs._locations;
	}
	return *this;
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

/**
 * @brief パスパターンが一致するLocationブロックを取得する
 *
 * @detail 前方一致で検索し、最も長い文字列がマッチしたLocationブロックを返す
 * @param path 検索するパスパターン
 * @return const LocationContext& 一致したLocationContext
 */
const LocationContext& ServerContext::getLocationContext(const std::string& path) const
{
	std::vector<LocationContext> locations = getLocations();

	std::vector<LocationContext>::iterator matched = locations.begin();
	std::string::size_type max = std::string::npos;

	for (std::vector<LocationContext>::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		// 前方一致の長さを取得する
		std::string::size_type currentMatch = it->getPath().find(path);
		if (currentMatch != std::string::npos && currentMatch > max)
		{
			max = currentMatch;
			matched = it;
		}
	}

	// マッチするLocationブロックがない場合はエラー
	if (max == 0)
	{
		throw std::runtime_error("Path not found");
	}

	return *matched;
}
