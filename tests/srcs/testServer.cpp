#include <gtest/gtest.h>
#include "Server.hpp"
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
	std::string content = "\"GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\n4\r\nWiki\r\n5\r\npedia\r\ne\r\n in\r\n\r\nchunks.\r\n0\r\n\r\n\"";
	std::system((command + " " + content).c_str());

	// ファイルからHTTPステータスコードを読み込む
	std::ifstream infile("./tests/status.log");
	int status;
	infile >> status;

	EXPECT_EQ(status, 200);
}

// 3. CGIのリクエストを受け付けるか
TEST_F(ServerTest, cgiRequest)
{

	std::string content = "\"GET /cgi/ HTTP/1.1\r\nHost: localhost\r\n\r\n\"";
	std::system((command + " " + content).c_str());

	// ファイルからHTTPステータスコードを読み込む
	std::ifstream infile("./tests/status.log");
	int status;
	infile >> status;

	EXPECT_EQ(status, 200);
}
};