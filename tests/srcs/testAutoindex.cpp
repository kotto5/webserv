#include <gtest/gtest.h>
#include "Autoindex.hpp"
#include "Config.hpp"
#include <iostream>
#include <fstream>

namespace
{
class AutoindexTest : public ::testing::Test
{
protected:
	const std::string path = "docs/";
	// テストデータの作成
	virtual void SetUp()
	{
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
	Autoindex autoindex(path);
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
