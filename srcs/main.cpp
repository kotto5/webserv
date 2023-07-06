#include "Request/Request.hpp"
#include "Server/server.hpp"
#include <iostream>
#include <map>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		return 1;
	}
	(void)argv;
	// 設定ファイル読み込み
	// サーバー起動
	Server server;
	server.setup();
	server.run();
	// サーバー終了

	return 0;
}