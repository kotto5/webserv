#include "Request.hpp"
#include "Config.hpp"
#include "ConfigException.hpp"
#include "server.hpp"
#include "Logger.hpp"
#include <iostream>
#include <map>
#include "ErrorCode.hpp"
#include "utils.hpp"
#include <signal.h>
#include "Request.hpp"
#include <filesystem>
#include "ErrorCode.hpp"
#include "ServerException.hpp"

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

void runServer()
{
	try
	{
		// ロギング初期化
		Logger::initialize(
			Config::instance()->getHTTPBlock().getAccessLogFile(),
			Config::instance()->getHTTPBlock().getErrorLogFile()
		);

		// シグナルハンドラ設定
		setSignalHandler();
		// サーバー起動
		Server server;
		server.setup();
		server.run();
	}
	catch(const ServerException &e)
	{
		Logger::instance()->writeErrorLog(ErrorCode::SYSTEM_CALL, e.what());
		Config::release();
		Logger::release();
		std::exit(1);
	}
}

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		return 1;
	}
	try
	{
		// 設定ファイル読み込み
		if (argc == 1)
			Config::initialize();
		else
			Config::initialize(argv[1]);
	}
	catch(const ConfigException& e)
	{
		std::cerr << e.what() << std::endl;
		std::exit(1);
	}
	// サーバー起動
	runServer();
	Config::release();
	Logger::release();
	return 0;
}
