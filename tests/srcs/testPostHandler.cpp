#include <gtest/gtest.h>
#include "Request.hpp"
#include "Config.hpp"
#include "PostHandler.hpp"

namespace
{
class PostHandlerTest : public ::testing::Test
{
protected:

	Config *config;

	// テストデータの作成
	virtual void SetUp()
	{
		config = new Config("conf/default.conf");
	}
};

// 1. リソースが正しく作成されるか（txt）
TEST_F(PostHandlerTest, createTextFile)
{
	// インスタンスの生成
	PostHandler handler;
	Request req("POST", "docs/test.html", "HTTP/1.1", std::map<std::string, std::string>(), "Hello World");
	Response res = handler.handleRequest(req);

	// テストデータの検証
	EXPECT_EQ(res.getStatus(), 201);
	EXPECT_EQ(res.getBody(), "");
	EXPECT_EQ(res.getHeader("Content-Type"), "text/html");
}

// 2. リソースが正しく作成されるか（png）
TEST_F(PostHandlerTest, createPngFile)
{
	PostHandler handler;
	Request req("POST", "docs/test.png", "HTTP/1.1", std::map<std::string, std::string>(), "Hello World");
	Response res = handler.handleRequest(req);

	// テストデータの検証
	EXPECT_EQ(res.getStatus(), 201);
	EXPECT_EQ(res.getBody(), "");
	EXPECT_EQ(res.getHeader("Content-Type"), "image/png");
}



}