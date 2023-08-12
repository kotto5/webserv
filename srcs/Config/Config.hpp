#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "HTTPContext.hpp"
#include <string>

class Config
{
public:
	static void initialize(const std::string& filepath = DEFAULT_CONFIG_FILEPATH);
	static void release();
	HTTPContext& getHTTPBlock();
	static Config* instance();
	const std::vector<std::string> getPorts();

private:
	HTTPContext _http_block;
	static Config* _instance;
	static const std::string DEFAULT_CONFIG_FILEPATH;

	// シングルトンパターンのため外部からの変更・破棄を避ける
	Config();
	Config(const std::string& filepath);
	~Config();
	Config& operator=(const Config& other);
	Config(const Config& other);
};

#endif