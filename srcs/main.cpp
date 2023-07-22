#include "Request/Request.hpp"
#include "config/Config.hpp"
#include "config/ConfigError.hpp"
#include "Server/server.hpp"
#include "Logger/Logger.hpp"
#include <iostream>
#include <map>
#include "Error.hpp"
#include "utils.hpp"
#include <signal.h>
#include "Request.hpp"
#include <filesystem>

// int	setSignalHandler()
// {
// 	struct sigaction	sa;
// 	sa.sa_handler = SIG_IGN;
// 	sigemptyset(&sa.sa_mask);
// 	sa.sa_flags = SA_RESTART;
// 	if (sigaction(SIGPIPE, &sa, NULL) == -1)
// 	{
// 		std::cout << "sigaction error" << std::endl;
// 		return (1);
// 	}
// 	return (0);
// }

// write exit_handler
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
	return (0);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		return 1;
	}
	// 設定ファイル読み込み
	Config	*config;
	try
	{
		config = new Config(argv[1]);
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

	setSignalHandler();
	// サーバー起動
	Server server;
	if (server.setup())
		return (1);
	server.run();
	// サーバー終了
	delete config;
	delete logger;

	return 0;
}

// int	main()
// {

// 	Config	*config;
// 	try
// 	{
// 		config = new Config("/Users/kakiba/AAproject/42_webserv/conf/default.conf");
// 	}
// 	catch(const ConfigError& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 		std::exit(1);
// 	}

// 	// open file text.txt
// 	std::ifstream ifs("/Users/kakiba/AAproject/42_webserv/sample_http/Request1");
// 	// std::ifstream ifs("/Users/kakiba/AAproject/42_webserv/sample_http/RequestPost1");
// 	if (!ifs)
// 	{
// 		std::cerr << "Error: file not opened." << std::endl;
// 		return 1;
// 	}
// 	std::string row_request;
// 	// read ifs to row_request
// 	row_request.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
// 	Request	*req = Server::parse_request(row_request);
// 	std::cout << "test@@@@@@@@@@@" << std::endl;
// 	if (req == NULL)
// 	{
// 		std::cout << "parse error" << std::endl;
// 		return (1);
// 	}
// 	req->print_all();
// }
