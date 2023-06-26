#include <gtest/gtest.h>
#include "Request.hpp"
#include "Router.hpp"
#include "GetHandler.hpp"

// 1. GetHandlerが取得されるか
TEST(RouterTest, createGetRequest)
{
	// テストデータの挿入
	Request rq("GET", "/index.html", std::map<std::string, std::string>(), "");
	// インスタンスの生成
	Router router;
	// テストデータの検証
	IHandler *handler = router.findHandler(rq);
	EXPECT_TRUE(handler != NULL);
}

// 2. postHandlerに正しくルーティングされるか