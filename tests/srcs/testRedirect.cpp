#include <gtest/gtest.h>
#include "Config.hpp"
#include <fstream>
#include "Server.hpp"

namespace
{
class RedirectTest: public ::testing::Test
{
protected:
	std::string command = "./tests/scripts/request.sh";

	// テストデータの作成
	virtual void SetUp()
	{
		Config::release();
		Config::initialize("./conf/testConfig/redirectTest.conf");
		std::system("./tests/scripts/run.sh");
	}
	virtual void TearDown()
	{
		std::system("./tests/scripts/stop.sh");
		Config::release();
		Config::initialize("./conf/testConfig/defaultTest.conf");
	}
};
}

// 1. GETリクエストを受け付けるか
TEST_F(RedirectTest, request)
{
	std::string content = "\"GET /redirect1/ HTTP/1.1\r\nHost: localhost\r\n\r\n\"";
	std::system((command + " " + content).c_str());

	// ファイルからHTTPステータスコードを読み込む
	std::ifstream infile("./tests/status.log");
	int status;
	infile >> status;

	EXPECT_EQ(status, 301);
}
