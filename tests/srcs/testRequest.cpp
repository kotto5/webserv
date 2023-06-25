#include <gtest/gtest.h>
#include "test.hpp"
#include "Request.hpp"

// 1. リクエストクラスが正しく生成されているか
TEST(RequestTest, getRequest)
{
	std::string method = "GET";
	std::string url = "/index.html";
	std::map<std::string, std::string> headers;
	headers.insert(std::make_pair("content-length", "100"));
	headers.insert(std::make_pair("content-type", "text/html"));
	std::string body = "Hello World";

	Request rq(method, url, headers, body);

	EXPECT_EQ(rq.getMethod(), "GET");
	EXPECT_EQ(rq.getUri(), "/index.html");
	EXPECT_EQ(rq.getHeader("content-length"), "100");
	EXPECT_EQ(rq.getHeader("content-type"), "text/html");
	EXPECT_EQ(rq.getBody(), "Hello World");
}

// 2. リクエストクラスが正しくコピーされているか
TEST(RequestTest, copyRequest)
{
	std::string method = "GET";
	std::string url = "/index.html";
	std::map<std::string, std::string> headers;
	headers.insert(std::make_pair("content-length", "100"));
	headers.insert(std::make_pair("content-type", "text/html"));
	std::string body = "Hello World";

	Request rq(method, url, headers, body);
	Request rq2(rq);

	EXPECT_EQ(rq.getMethod(), "GET");
	EXPECT_EQ(rq.getUri(), "/index.html");
	EXPECT_EQ(rq.getHeader("content-length"), "100");
	EXPECT_EQ(rq.getHeader("content-type"), "text/html");
	EXPECT_EQ(rq.getBody(), "Hello World");
}