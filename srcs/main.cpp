#include <iostream>

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
	// サーバー終了

	return 0;
}