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
	Request *reqNoMethod;
	Request *reqNoHostHeader;

	// テストデータの作成
	virtual void SetUp()
	{
		reqGet = new Request();
		reqGet->parsing("GET /index.html HTTP/1.1\r\nHost: 0.0.0.0\r\n\r\n", 0);
		reqGet->setAddr(env->_test_clientSocket).setInfo();

		reqPost = new Request();
		reqPost->parsing("POST /storage/unit_test/router_test.txt HTTP1.1\r\nHost: 0.0.0.0\r\n\r\n", 0);
		reqPost->setAddr(env->_test_clientSocket).setInfo();

		reqDelete = new Request();
		reqDelete->parsing("DELETE /index.html HTTP/1.1\r\nHost: 0.0.0.0\r\n\r\n", 0);
		reqDelete->setAddr(env->_test_clientSocket).setInfo();

		reqNoMethod = new Request();
		reqNoMethod->parsing("NONE /index.html HTTP/1.1\r\nHost: 0.0.0.0\r\n\r\n", 0);
		reqNoMethod->setAddr(env->_test_clientSocket).setInfo();

		reqNoHostHeader = new Request();
		reqNoHostHeader->parsing("GET /index.html HTTP/1.1\r\n\r\n", 0);
		reqNoHostHeader->setAddr(env->_test_clientSocket).setInfo();
	}
};

// 1. GetHandlerが取得されるか
TEST_F(RouterTest, createGetRequest)
{
	// インスタンスの生成
	Router router;
	// テストデータの検証
	Response *response = (Response *)router.routeHandler(*reqGet, env->_test_clientSocket);
	EXPECT_EQ(response->getStatus(), "200");
}

// 2. 該当するハンドラーがない場合、405が返されるか
TEST_F(RouterTest, notRequest)
{
	// インスタンスの生成
	Router router;
	// テストデータの検証
	Response *response = (Response *)router.routeHandler(*reqNoMethod, env->_test_clientSocket);
	EXPECT_EQ(response->getStatus(), "405");
}

// 3. 許可されているメソッドのみ受けつけるか
TEST_F(RouterTest, allowedMethod)
{
	// インスタンスの生成
	Router router;
	// テストデータの検証
	Response *res1 = (Response *)router.routeHandler(*reqGet, env->_test_clientSocket);
	EXPECT_EQ(res1->getStatus(), "200");
	Response *res2 = (Response *)router.routeHandler(*reqPost, env->_test_clientSocket);
	EXPECT_EQ(res2->getStatus(), "201");
	Response *res3 = (Response *)router.routeHandler(*reqDelete, env->_test_clientSocket);
	EXPECT_EQ(res3->getStatus(), "405");
}
};

TEST_F(RouterTest, noHostHeader)
{
	// インスタンスの生成
	Router router;
	// テストデータの検証
	Response *response = (Response *)router.routeHandler(*reqNoHostHeader, env->_test_clientSocket);
	EXPECT_EQ(response->getStatus(), "400");
}