#include "Request/Request.hpp"
#include "config/Config.hpp"
#include "Server/server.hpp"
#include <iostream>
#include <map>
#include "utils.hpp"

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
