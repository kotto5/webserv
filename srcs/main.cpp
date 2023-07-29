#include "Request/Request.hpp"
#include "config/Config.hpp"
#include "config/ConfigError.hpp"
#include "Server/server.hpp"
#include "Logger/Logger.hpp"
#include <iostream>
#include <map>
#include "ErrorCode.hpp"
#include "utils.hpp"
#include <signal.h>
#include "Request.hpp"
#include <filesystem>

void	exit_handler(int sig)
{
	(void)sig;
	std::cout << "exit_handler" << std::endl;
	system("leaks -q webserv");
	exit(0);
}

int	setSignalHandler()
{
	struct sigaction	sa;
	sa.sa_handler = exit_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		std::cout << "sigaction error" << std::endl;
		return (1);
	}

	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &sa, NULL) == -1)
	{
		std::cout << "sigaction error" << std::endl;
		return (1);
	}
	return (0);
}

int main(int argc, char **argv)
{	if (argc != 2)
	{
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		return 1;
	}
	// 設定ファイル読み込み
	Config	*config;
	try
	{
		config = new Config(argv[1]);
		(void)config;
	}
	catch(const ConfigError& e)
	{
		std::cerr << e.what() << '\n';
		std::exit(1);
	}

	// ロギング設定
	Logger *logger = new Logger(
		Config::getInstance()->getHTTPBlock().getAccessLogFile(),
		Config::getInstance()->getHTTPBlock().getErrorLogFile()
	);
	(void)logger;

	setSignalHandler();
	// サーバー起動
	Server server;
	if (server.setup())
		return (1);
	server.run();

	return 0;
}
