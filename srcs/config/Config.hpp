#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "HTTPContext.hpp"
#include <string>

class Config
{
public:
	Config(const std::string &filepath);
	~Config();
	HTTPContext &getHTTPBlock();
	static Config *getInstance();

private:
	HTTPContext _http_block;
	static Config *_instance;
};

#endif