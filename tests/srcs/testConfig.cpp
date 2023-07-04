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
}
