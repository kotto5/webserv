#include <gtest/gtest.h>
#include "Config.hpp"

namespace
{
class ConfigTest : public ::testing::Test
{
protected:
	// テストデータの作成
	virtual void SetUp()
	{
	}
	// テストデータの破棄
	virtual void TearDown()
	{
	}
};

// 1. インスタンスが正しく生成されているか
TEST_F(ConfigTest, ConfigSetup)
{
	EXPECT_TRUE(Config::instance() != NULL);
}

// 2. インスタンスが取得でき、かつシングルトンであるか
TEST_F(ConfigTest, getInstance)
{
	Config *config1 = Config::instance();
	EXPECT_TRUE(config1 != NULL);

	// 両者は同一のインスタンスである
	Config *config2 = Config::instance();
	EXPECT_EQ(config1, config2);
}

// 3. HTTPブロックの要素が解析できているか
TEST_F(ConfigTest, getHTTPBlock)
{
	HTTPContext httpBlock = Config::instance()->getHTTPBlock();
	EXPECT_EQ(httpBlock.getAccessLogFile(), "./logs/access.log");
	EXPECT_EQ(httpBlock.getErrorLogFile(), "./logs/error.log");
}

// 4. Serverブロックの要素が解析できているか
TEST_F(ConfigTest, getServerBlock)
{
	HTTPContext httpBlock = Config::instance()->getHTTPBlock();
	const std::map<std::string, std::vector<ServerContext> > servers = httpBlock.getServers();
	std::vector<ServerContext> serverContexts = servers.at("80");
	EXPECT_EQ(serverContexts[0].getListen(), "80");
	EXPECT_EQ(serverContexts[0].getServerName(), "webserve1");
	EXPECT_EQ(serverContexts[0].getErrorPage("403"), "/error_page/403.html");
	EXPECT_EQ(serverContexts[0].getErrorPage("404"), "/error_page/404.html");
	EXPECT_EQ(serverContexts[0].getErrorPage("501"), "/error_page/50x.html");
	EXPECT_EQ(serverContexts[0].getErrorPage("502"), "/error_page/50x.html");
	EXPECT_EQ(serverContexts[0].getErrorPage("503"), "/error_page/50x.html");
	EXPECT_EQ(serverContexts[0].getErrorPage("504"), "/error_page/50x.html");
}

// 5. Locationブロックの要素が解析できているか
TEST_F(ConfigTest, getLocationBlock)
{
	HTTPContext httpBlock = Config::instance()->getHTTPBlock();
	const std::map<std::string, std::vector<ServerContext> > servers = httpBlock.getServers();
	std::vector<ServerContext> serverContexts = servers.at("80");
	std::vector<LocationContext> locationContexts = serverContexts[0].getLocations();
	EXPECT_EQ(locationContexts[0].getDirective("path"), "/");
	EXPECT_EQ(locationContexts[0].getDirective("alias"), "./docs");
	EXPECT_EQ(locationContexts[0].getDirective("index"), "index.html");
}

// 6. ポート番号が一致するServerブロックをすべて取得できるか
TEST_F(ConfigTest, getServerContexts)
{
	HTTPContext httpBlock = Config::instance()->getHTTPBlock();
	const ServerContext &sc = httpBlock.getServerContext("80", "webserve1");
	const LocationContext lc = sc.getLocationContext("/");
	EXPECT_EQ(lc.getDirective("path"), "/");
	EXPECT_EQ(lc.getDirective("alias"), "./docs");
	EXPECT_EQ(lc.getDirective("index"), "index.html");
	EXPECT_EQ(lc.getDirective("autoindex"), "on");

}

// 7. ポート番号が同一でserver_nameが異なるブロックを取得できるか
TEST_F(ConfigTest, getServerContextsAsSamePort)
{
	HTTPContext httpBlock = Config::instance()->getHTTPBlock();
	const ServerContext &sc = httpBlock.getServerContext("80", "webserv2.com");
	const LocationContext lc = sc.getLocationContext("/");
	EXPECT_EQ(lc.getDirective("alias"), "./docs");
	EXPECT_EQ(lc.getDirective("index"), "index.html");
}

// 8.ポート番号とserver_nameが異なるブロックを取得できるか
TEST_F(ConfigTest, getServerContextsAsOtherPort)
{
	HTTPContext httpBlock = Config::instance()->getHTTPBlock();
	const ServerContext &sc = httpBlock.getServerContext("81", "webserv3.com");
	const LocationContext lc = sc.getLocationContext("/");
	EXPECT_EQ(lc.getDirective("alias"), "./docs");
	EXPECT_EQ(lc.getDirective("index"), "index.html");
}
}