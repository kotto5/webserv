#include <gtest/gtest.h>
#include "Request.hpp"

// 1. リクエストクラスが正しく生成されているか
TEST(RequestTest, getRequest)
{
	// テストデータの挿入
	std::string method = "GET";
	std::string url = "/index.html";
	std::string protocol = "HTTP/1.1";
	std::map<std::string, std::string> headers;
	headers.insert(std::make_pair("content-length", "100"));
	headers.insert(std::make_pair("content-type", "text/html"));
	std::string body = "Hello World";

	// インスタンスの生成
	Request rq(method, url, protocol, headers, body);

	// テストデータの検証
	EXPECT_EQ(rq.getMethod(), "GET");
	EXPECT_EQ(rq.getUri(), "/index.html");
	EXPECT_EQ(rq.getHeader("content-length"), "100");
	EXPECT_EQ(rq.getHeader("content-type"), "text/html");
	EXPECT_EQ(rq.getBody(), "Hello World");
}

// 2. リクエストクラスが正しくコピーされているか
TEST(RequestTest, copyRequest)
{
	// テストデータの挿入
	std::string method = "GET";
	std::string url = "/index.html";
	std::string protocol = "HTTP/1.1";
	std::map<std::string, std::string> headers;
	headers.insert(std::make_pair("content-length", "100"));
	headers.insert(std::make_pair("content-type", "text/html"));
	std::string body = "Hello World";

	// インスタンスの生成とコピー
	Request rq(method, url, protocol, headers, body);
	Request rq2(rq);

	// テストデータの検証
	EXPECT_EQ(rq.getMethod(), "GET");
	EXPECT_EQ(rq.getUri(), "/index.html");
	EXPECT_EQ(rq.getHeader("content-length"), "100");
	EXPECT_EQ(rq.getHeader("content-type"), "text/html");
	EXPECT_EQ(rq.getBody(), "Hello World");
}