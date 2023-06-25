#include <gtest/gtest.h>
#include "Response.hpp"

// 1. レスポンスクラスが正しく生成されているか
TEST(ResponseTest, getResponse)
{
	// テストデータの挿入
	int statusCode = 200;
	std::map<std::string, std::string> headers;
	headers.insert(std::make_pair("content-length", "100"));
	headers.insert(std::make_pair("content-type", "text/html"));
	std::string body = "Hello World";

	// インスタンスの生成
	Response res(statusCode, headers, body);

	// テストデータの検証
	EXPECT_EQ(res.getStatus(), 200);
	EXPECT_EQ(res.getHeader("content-length"), "100");
	EXPECT_EQ(res.getHeader("content-type"), "text/html");
	EXPECT_EQ(res.getBody(), "Hello World");
}

// 2. ステータスメッセージの取得が正しく行われているか
TEST(ResponseTest, getStatusMessage)
{
	// テストデータの挿入
	std::map<std::string, std::string> headers;
	std::string body = "";
	// インスタンスの生成
	Response res0(200, headers, body);
	Response res1(400, headers, body);
	Response res2(404, headers, body);

	std::string raw0 = res0.toString();
	std::string raw1 = res1.toString();
	std::string raw2 = res2.toString();

	// テストデータの検証
	EXPECT_TRUE(raw0.find("OK") != std::string::npos);
	EXPECT_TRUE(raw1.find("Bad Request") != std::string::npos);
	EXPECT_TRUE(raw2.find("Not Found") != std::string::npos);
}

// 3. 平文への変換が正しく行われているか
TEST(ResponseTest, toString)
{
	// テストデータの挿入
	int statusCode = 200;
	std::map<std::string, std::string> headers;
	headers.insert(std::make_pair("content-length", "100"));
	headers.insert(std::make_pair("content-type", "text/html"));
	std::string body = "Hello World";

	// インスタンスの生成
	Response res(statusCode, headers, body);

	// テストデータの検証
	EXPECT_EQ(res.toString(), "HTTP/1.1 200 OK\r\ncontent-length: 100\r\ncontent-type: text/html\r\n\r\nHello World");
}
