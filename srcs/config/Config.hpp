#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "HTTPContext.hpp"
#include <string>

class Config
{
public:
	Config(const std::string& filepath);
	HTTPContext& getHTTPBlock();
	static Config* getInstance();
	const std::vector<std::string> getPorts();

private:
	HTTPContext _http_block;
	static Config* _instance;

	// シングルトンパターンのため外部からの変更・破棄を避ける
	~Config();
	Config& operator=(const Config& other);
	Config(const Config& other);
};

#endif