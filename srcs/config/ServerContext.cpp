#include "ServerContext.hpp"
#include <stdexcept>
#include <iostream>

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

void ServerContext::setListen(const std::string& listen)
{
	_listen = listen;
}

void ServerContext::setServerName(const std::string& server_name)
{
	_server_name = server_name;
}

const std::string& ServerContext::getListen() const
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
	const std::vector<LocationContext>& locations = getLocations();

	bool isMatched = false;
	std::vector<LocationContext>::const_iterator matched = locations.begin();
	std::string::size_type max = 0;
	for (std::vector<LocationContext>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		// 前方一致の長さを取得する
		std::string::size_type currentMatch = it->getDirective("path").find(path);
		if (currentMatch != std::string::npos)
		{
			if (!isMatched || max < currentMatch)
			{
				max = currentMatch;
				matched = it;
				isMatched = true;
			}
		}
	}
	if (!isMatched)
	{
		throw std::runtime_error("No matching LocationContext found for the given path.");
	}
	return *matched;
}
