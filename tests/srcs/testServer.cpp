#include <gtest/gtest.h>
#include "server.hpp"
#include <fstream>

namespace
{
class ServerTest: public ::testing::Test
{
protected:
	std::string command = "./tests/scripts/request.sh";
	std::string arg = "GET /docs/index.html Host:server";

	// テストデータの作成
	virtual void SetUp()
	{
		std::system("./tests/scripts/run.sh");
	}
	virtual void TearDown()
	{
		std::system("./tests/scripts/stop.sh");
	}
};

// 1. GETリクエストを受け付けるか
TEST_F(ServerTest, requestServer)
{
	std::system((command + " " + arg).c_str());

	// ファイルからHTTPステータスコードを読み込む
	std::ifstream infile("./tests/status_code.txt");
	int status;
	infile >> status;

	EXPECT_EQ(status, 200);
}
};