#include "GetHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Router.hpp"
#include <gtest/gtest.h>

namespace
{
class RouterTest : public ::testing::Test
{
protected:
	Request *reqGet;
	Request *reqNotFound;

	// テストデータの作成
	virtual void SetUp()
	{
		reqGet = new Request("GET", "/index.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqNotFound = new Request("NONE", "/index.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
	}
};

// 1. GetHandlerが取得されるか
TEST_F(RouterTest, createGetRequest)
{
	// インスタンスの生成
	Router router;
	// テストデータの検証
	Response *response = router.routeHandler(*reqGet);
	EXPECT_EQ(response->getStatus(), "200");
}

// 2. 該当するハンドラーがない場合、405が返されるか
TEST_F(RouterTest, notRequest)
{
	// インスタンスの生成
	Router router;
	// テストデータの検証
	Response *response = router.routeHandler(*reqNotFound);
	EXPECT_EQ(response->getStatus(), "405");
}
};