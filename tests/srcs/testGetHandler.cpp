#include <gtest/gtest.h>
#include "Request.hpp"
#include "Router.hpp"
#include "GetHandler.hpp"
#include "Config.hpp"

namespace
{
class GetHandlerTest : public ::testing::Test
{
protected:
	Request *reqGetHtml;
	Request *reqGetPng;
	Request *reqNotFound;
	const std::string expected = "<title>Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.</title>";

	// テストデータの作成
	virtual void SetUp()
	{
		reqGetHtml = new Request("GET", "/pages/test.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqGetPng = new Request("GET", "/images/logo.png", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqNotFound = new Request("GET", "/pages/none.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
	}
};

// 1. HTMLファイルを正しく取得できるか
TEST_F(GetHandlerTest, getHtmlFile)
{
	// テストデータの検証
	GetHandler handler;
	Response res = handler.handleRequest(*reqGetHtml);

	EXPECT_EQ(res.getStatus(), 200);
	EXPECT_EQ(res.getBody(), expected);
	EXPECT_EQ(res.getHeader("Content-Type"), "text/html");
}

// 2. PNGファイルを正しく取得できるか
TEST_F(GetHandlerTest, getPngFile)
{
	// テストデータの検証
	GetHandler handler;
	Response res = handler.handleRequest(*reqGetPng);

	EXPECT_EQ(res.getStatus(), 200);
	EXPECT_EQ(res.getHeader("Content-Type"), "image/png");
}

// 2. ファイルが存在しない場合、404が返されるか
TEST_F(GetHandlerTest, notRequest)
{
	// インスタンスの生成
	GetHandler handler;
	// テストデータの検証
	Response res = handler.handleRequest(*reqNotFound);
	EXPECT_EQ(res.getStatus(), 404);
	EXPECT_EQ(res.getBody(), "");
};
};