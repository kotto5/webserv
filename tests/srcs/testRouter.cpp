#include "GetHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Router.hpp"
#include <gtest/gtest.h>
#include "TestEnv.hpp"

namespace
{
class RouterTest : public ::testing::Test
{
protected:
	Request *reqGet;
	Request *reqPost;
	Request *reqDelete;
	Request *reqNotFound;

	// テストデータの作成
	virtual void SetUp()
	{
		reqGet = new Request("GET", "/index.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqGet->setAddr(env->socket).setInfo();
		reqPost = new Request("POST", "/storage/unit_test/router_test.txt", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqPost->setAddr(env->socket).setInfo();
		reqDelete = new Request("DELETE", "/index.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqDelete->setAddr(env->socket).setInfo();
		reqNotFound = new Request("NONE", "/index.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqNotFound->setAddr(env->socket).setInfo();
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

// 3. 許可されているメソッドのみ受けつけるか
TEST_F(RouterTest, allowedMethod)
{
	// インスタンスの生成
	Router router;
	// テストデータの検証
	Response *res1 = router.routeHandler(*reqGet);
	EXPECT_EQ(res1->getStatus(), "200");
	Response *res2 = router.routeHandler(*reqPost);
	EXPECT_EQ(res2->getStatus(), "201");
	Response *res3 = router.routeHandler(*reqDelete);
	EXPECT_EQ(res3->getStatus(), "405");
}
};