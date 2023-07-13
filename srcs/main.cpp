#include "Request/Request.hpp"
#include "config/Config.hpp"
#include "Server/server.hpp"
#include <iostream>
#include <map>
#include "utils.hpp"
#include <signal.h>


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
	Config *config = new Config(argv[1]);
	(void)config;
	setSignalHandler();
	// サーバー起動
	Server server;
	server.setup();
	server.run();
	// サーバー終了
	delete config;

	return 0;
}

// int	main()
// {
// 	int	sockets[2];
// 	if (_socketpair(AF_INET, SOCK_STREAM, 0, sockets) == -1)
// 	{
// 		std::cout << "socketpair error" << std::endl;
// 		exit (-1);
// 	}
// 	return (0);
// }
