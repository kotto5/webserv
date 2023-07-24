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
		~Config();

    private:
        HTTPContext _http_block;
        static Config* _instance;
        int dup2_x(int oldfd, int newfd);
        int newfd;
		int	setErrorLogFileStderror(std::string errorLogFile);

		// シングルトンパターンのため外部からの変更・破棄を避ける
		// ~Config();
		Config(const Config& other);
		Config& operator=(const Config& other);
};

#endif