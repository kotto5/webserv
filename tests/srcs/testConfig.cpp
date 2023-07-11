#include <gtest/gtest.h>
#include "Config.hpp"

namespace
{
class ConfigTest : public ::testing::Test
{
protected:
	Config *config;
	// テストデータの作成
	virtual void SetUp()
	{
		config = new Config("conf/default.conf");
	}
};

// 1. インスタンスが正しく生成されているか
TEST_F(ConfigTest, ConfigSetup)
{
	EXPECT_TRUE(config != NULL);
}

// 2. インスタンスが取得でき、かつシングルトンであるか
TEST_F(ConfigTest, getInstance)
{
	Config *config = Config::getInstance();
	EXPECT_TRUE(config != NULL);

	// 両者は同一のインスタンスである
	Config *config2 = Config::getInstance();
	EXPECT_TRUE(config == config2);
}

// 3. HTTPブロックの要素が解析できているか
TEST_F(ConfigTest, getHTTPBlock)
{
	HTTPContext httpBlock = config->getHTTPBlock();
	EXPECT_EQ(httpBlock.getAccessLogFile(), "./logs/access.log");
	EXPECT_EQ(httpBlock.getErrorLogFile(), "./logs/error.log");
}

// 4. Serverブロックの要素が解析できているか
TEST_F(ConfigTest, getServerBlock)
{
	HTTPContext httpBlock = config->getHTTPBlock();
	const std::map<std::string, std::vector<ServerContext> > servers = httpBlock.getServers();
	std::vector<ServerContext> serverContexts = servers.at("80");
	EXPECT_EQ(serverContexts[0].getListen(), "80");
	EXPECT_EQ(serverContexts[0].getServerName(), "webserve1");
}

// 5. Locationブロックの要素が解析できているか
TEST_F(ConfigTest, getLocationBlock)
{
	HTTPContext httpBlock = config->getHTTPBlock();
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
	HTTPContext httpBlock = config->getHTTPBlock();
	const ServerContext &sc = httpBlock.getServerContext("80", "webserve1");
	const LocationContext lc = sc.getLocationContext("/");
	EXPECT_EQ(lc.getDirective("path"), "/");
	EXPECT_EQ(lc.getDirective("alias"), "./docs");
	EXPECT_EQ(lc.getDirective("index"), "index.html");
}
}