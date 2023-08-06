#include "ServerContext.hpp"
#include "ConfigException.hpp"
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

ServerContext::ServerContext(const ServerContext& other)
{
	*this = other;
}

ServerContext& ServerContext::operator=(const ServerContext& other)
{
	if (this != &other)
	{
		_listen = other._listen;
		_server_name = other._server_name;
		_locations = other._locations;
		_directives = other._directives;
		_error_pages = other._error_pages;
	}
	return *this;
}

void ServerContext::setListen(const std::string& listen)
{
	_listen = listen;
}

void ServerContext::setServerName(const std::string& server_name)
{
	_server_name = server_name;
}

/**
 * @brief エラーページをセットする
 *
 * @param error_pages
 */
void ServerContext::setErrorPages(const std::vector<std::string> &error_pages)
{
	if (error_pages.size() < 2)
	{
		throw ConfigException(ErrorCode::CONF_UNKOWN_DIRECTIVE, "ErrorPage syntax");
	}
	// 最後-1の要素をパスとして格納
	std::string path = error_pages[error_pages.size() - 2];
	for (std::vector<std::string>::size_type i = 1; i + 1 < error_pages.size(); i++)
	{
		std::cout << "error_pages[" << i << "] = " << error_pages[i] << std::endl;
		// ステータスコード, パスの順に格納
		_error_pages.insert(std::make_pair(error_pages[i], path));
	}
}

const std::string& ServerContext::getListen() const
{
	return _listen;
}

const std::string& ServerContext::getServerName() const
{
	return _server_name;
}

/**
 * @brief エラーページを取得する
 *
 * @param status
 * @return const std::string&
 */
std::string ServerContext::getErrorPage(const std::string &status) const
{
	std::map<std::string, std::string>::const_iterator it = _error_pages.find(status);
	if (it != _error_pages.end()) {
		return it->second;
	}
	return "";
}

void ServerContext::addLocationBlock(const LocationContext& location)
{
	_locations.push_back(location);
}

void ServerContext::addDirectives(const std::string& directive, const std::string& value,
	const std::string& filepath, int line_number)
{
	// check if directive is not duplicated
	if (_directives.find(directive) != _directives.end())
		throw ConfigException(ErrorCode::CONF_DUPLICATE_DIRECTIVE, directive, filepath, line_number);

	_directives.insert(std::make_pair(directive, value));
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
	std::string	fixPath = path;
	if (!path.empty() && fixPath[fixPath.length() - 1] != '/')
		fixPath += '/';
	const std::vector<LocationContext>& locations = getLocations();

	bool isMatched = false;
	std::vector<LocationContext>::const_iterator matched = locations.begin();
	std::string::size_type max = 0;
	for (std::vector<LocationContext>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		// ロケーションパスを取得
		std::string locationPath = it->getDirective("path");
		std::string::size_type currentMatch = 0;
		// 前方一致の最大の長さを取得する
		if (fixPath.size() >= locationPath.size() &&
			std::equal(std::begin(locationPath), std::end(locationPath), std::begin(fixPath)))
			currentMatch = locationPath.size();
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

/**
 * @brief 前方一致する最大の長さを求める
 *
 * @detail
 * @param str1 比較する文字列1
 * @param str2 比較する文字列2
 * @return std::string::size_type 前方一致する最大の長さ
 */
std::string::size_type ServerContext::getMaxPrefixLength(const std::string& str1, const std::string& str2) const
{
	std::string::size_type i = 0;
	while (i < str1.size() && i < str2.size() && str1[i] == str2[i])
	{
		++i;
	}
	return i;
}
