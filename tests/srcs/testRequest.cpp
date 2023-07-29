#include <gtest/gtest.h>
#include "Request.hpp"
#include "Config.hpp"

namespace
{
class RequestTest : public ::testing::Test
{
protected:
	// テストデータの挿入
	std::string method = "GET";
	std::string url = "/index.html";
	std::string protocol = "HTTP/1.1";
	std::map<std::string, std::string> headers;
	std::string body = "Hello World";

	// テストデータの作成
	virtual void SetUp()
	{
		headers.insert(std::make_pair("content-length", "100"));
		headers.insert(std::make_pair("content-type", "text/html"));
	}
};

// 1. リクエストクラスが正しく生成されているか
TEST_F(RequestTest, getRequest)
{
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
TEST_F(RequestTest, copyRequest)
{
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

// 3.　aliasが正しく反映されるか
TEST_F(RequestTest, alias)
{
	// インスタンスの生成
	url = "/";
	Request rq1(method, url, protocol, headers, body);
	url = "/index.html";
	Request rq2(method, url, protocol, headers, body);
	url = "/resources/";
	Request rq3(method, url, protocol, headers, body);
	url = "/resources/index.html";
	Request rq4(method, url, protocol, headers, body);

	// テストデータの検証
	EXPECT_EQ(rq1.getActualUri(), "./docs/index.html");
	EXPECT_EQ(rq2.getActualUri(), "./docs/index.html");
	EXPECT_EQ(rq3.getActualUri(), "./data/s3/resources/index.html");
	EXPECT_EQ(rq4.getActualUri(), "./data/s3/resources/index.html");
}

}