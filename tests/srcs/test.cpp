#include <gtest/gtest.h>
#include "Config.hpp"

class Env : public testing::Environment {
public:
	virtual ~Env() {}
	// 環境の初期化方法を定義するには，これをオーバーライドしてください．
	virtual void SetUp()
	{
		Config::initialize("./conf/default.conf");
	}
	virtual void TearDown() {}
};

// テスト初期化
int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	AddGlobalTestEnvironment(new Env);
	return RUN_ALL_TESTS();
}
