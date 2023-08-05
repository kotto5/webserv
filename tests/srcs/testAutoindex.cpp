#include <gtest/gtest.h>
#include "Autoindex.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include <iostream>
#include <fstream>

namespace
{
class AutoindexTest : public ::testing::Test
{
protected:
	std::string method = "GET";
	std::string url = "/pages";
	std::string protocol = "HTTP/1.1";
	std::map<std::string, std::string> headers;

	// テストデータの作成
	virtual void SetUp()
	{
		headers.insert(std::make_pair("content-type", "text/html"));
	}
	// テストデータの破棄
	virtual void TearDown()
	{
	}
};

// 1. Autoindexの設定値を取得できるか
TEST_F(AutoindexTest, getAutoindex)
{
	bool result = Config::instance()->getHTTPBlock()
		.getServerContext("80", "webserve1")
		.getLocationContext("/")
		.getDirective("autoindex") == "on";
	EXPECT_TRUE(result);
}

TEST_F(AutoindexTest, generateAutoindex)
{
	Request req(method, url, protocol, headers, "");

	Autoindex autoindex(req);
	std::string html = autoindex.generateAutoindex();

	std::ofstream ofs("autoindex_sample.html");
	ofs << html;
	ofs.close();

	EXPECT_TRUE(html.find("<html>") != std::string::npos);
	EXPECT_TRUE(html.find("</html>") != std::string::npos);
	EXPECT_TRUE(html.find("<head>") != std::string::npos);
	EXPECT_TRUE(html.find("</head>") != std::string::npos);
	EXPECT_TRUE(html.find("<body>") != std::string::npos);
	EXPECT_TRUE(html.find("</body>") != std::string::npos);
}
}
