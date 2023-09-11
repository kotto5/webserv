#include <gtest/gtest.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "TestEnv.hpp"

TestEnv* env = NULL;
int	sigStatus = 0;

// テスト初期化
int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	env = new TestEnv();
	::testing::AddGlobalTestEnvironment(env);
	return RUN_ALL_TESTS();
}
