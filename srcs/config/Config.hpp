#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include "HTTPContext.hpp"

class Config
{
    public:
        Config(const std::string& filepath);
        ~Config();
		HTTPContext& getHTTPBlock();

	private:
		HTTPContext _http_block;
};

#endif