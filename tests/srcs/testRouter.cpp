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
		reqGet->setAddr(env->_test_clientSocket).setInfo().parsing("\r\n\r\n", 0);
		reqPost = new Request("POST", "/storage/unit_test/router_test.txt", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqPost->setAddr(env->_test_clientSocket).setInfo().parsing("\r\n\r\n", 0);
		reqDelete = new Request("DELETE", "/index.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqDelete->setAddr(env->_test_clientSocket).setInfo().parsing("\r\n\r\n", 0);
		reqNotFound = new Request("NONE", "/index.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqNotFound->setAddr(env->_test_clientSocket).setInfo().parsing("\r\n\r\n", 0);
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
	Response *response = (Response *)router.routeHandler(*reqNotFound, env->_test_clientSocket);
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