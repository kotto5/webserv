#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "HTTPContext.hpp"
#include <string>

class Config
{
    public:
        Config(const std::string& filepath);
        ~Config();
		HTTPContext& getHTTPBlock();
		static Config* getInstance();
		const std::vector<std::string>& getPorts();

    private:
        HTTPContext _http_block;
        static Config* _instance;
        int dup2_x(int oldfd, int newfd);
        int newfd;
		int	setErrorLogFileStderror(std::string errorLogFile);
};

#endif