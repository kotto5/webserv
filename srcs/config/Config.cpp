#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ConfigError.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>

Config::Config(const std::string& filepath)
{
	ConfigParser parser(*this);
	parser.parseFile(filepath);
	_instance = this;

	std::string errorLogFile = getHTTPBlock().getErrorLogFile();
	redirectErrorLogFile(errorLogFile);

	std::string accessLogFile = getHTTPBlock().getAccessLogFile();
	redirectAccessLogFile(accessLogFile);
}

int	Config::redirectErrorLogFile(std::string errorLogFile)
{
	if (errorLogFile.empty())
	{
		return (0);
	}
	// エラーログファイルを開く（appendモード）
	std::ofstream ofs(errorLogFile.c_str(), std::ios::app);

	// 標準エラー出力をリダイレクトする
	std::streambuf* original_cerr = std::cerr.rdbuf(ofs.rdbuf());
	(void)original_cerr;
	return (0);
}

int Config::redirectAccessLogFile(std::string  accessLogFile)
{
	if (accessLogFile.empty())
	{
		return (0);
	}

	// アクセスログファイルを開く（appendモード）
	std::ofstream ofs(accessLogFile.c_str(), std::ios::app);

	// 標準エラー出力をリダイレクトする
	std::streambuf* original_cerr = std::cerr.rdbuf(ofs.rdbuf());
	(void)original_cerr;
	return (0);
}

HTTPContext& Config::getHTTPBlock()
{
	return _http_block;
}

Config* Config::getInstance()
{
	return _instance;
}

/**
 * @brief 設定されているポートをすべて取得
 *
 * @return const std::vector<std::string>
 */
const std::vector<std::string> Config::getPorts()
{
	std::vector<std::string> ports;
	std::map<std::string, std::vector<ServerContext> >::const_iterator it;

	for (it = _http_block.getServers().begin(); it != _http_block.getServers().end(); ++it)
	{
		for (std::vector<ServerContext>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			std::cout << "TEST" << std::endl;
			// Directly add the listen value as the port, since it should only contain the port number
			ports.push_back(it2->getListen());
		}
	}
	return ports;
}

// シングルトンパターンのため外部からの変更・破棄を避ける
Config::~Config()
{
}

Config::Config(const Config& other)
{
	*this = other;
}

Config& Config::operator=(const Config& other)
{
	if (this != &other)
	{
		_http_block = other._http_block;
	}
	return *this;
}

Config* Config::_instance = NULL;