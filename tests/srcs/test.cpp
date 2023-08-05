#include <gtest/gtest.h>
#include <unistd.h>
#include "Config.hpp"
#include "Logger.hpp"

class Env : public testing::Environment {
public:
	virtual ~Env() {}

	virtual void SetUp()
	{
		// 設定ファイルの読み込み
		Config::initialize("./conf/default.conf");
		Logger::initialize("./logs/ut_access.log", "./logs/ut_error.log");

		// テスト用ディレクトリを作成
		std::string command = "./tests/scripts/clean.sh";
		std::string path = "docs/storage/unit_test";
		std::system((command + " " + path).c_str());
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
