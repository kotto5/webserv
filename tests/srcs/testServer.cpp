#include <gtest/gtest.h>
#include "server.hpp"
#include <fstream>

namespace
{
class ServerTest: public ::testing::Test
{
protected:
	std::string command = "./tests/scripts/request.sh";

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
TEST_F(ServerTest, request)
{
	std::string content = "\"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n\"";
	std::system((command + " " + content).c_str());

	// ファイルからHTTPステータスコードを読み込む
	std::ifstream infile("./tests/status.log");
	int status;
	infile >> status;

	EXPECT_EQ(status, 200);
}

// 2. chunked形式のリクエストを受け付けるか
TEST_F(ServerTest, chunkedRequest)
{
	std::string content = "\"GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\nTransfer-Encoding: chunked\r\n\r\n\5\r\nHello\r\n5\r\nWorld\r\n0\r\n\r\"";
	std::system((command + " " + content).c_str());

	// ファイルからHTTPステータスコードを読み込む
	std::ifstream infile("./tests/status.log");
	int status;
	infile >> status;

	EXPECT_EQ(status, 200);
}

// 2. CGIのリクエストを受け付けるか
TEST_F(ServerTest, cgiRequest)
{

	std::string content = "\"GET /docs/index.php HTTP/1.1\r\nHost: localhost\r\n\r\n\"";
	std::system((command + " " + content).c_str());

	// ファイルからHTTPステータスコードを読み込む
	std::ifstream infile("./tests/status.log");
	int status;
	infile >> status;

	EXPECT_EQ(status, 200);
}
};