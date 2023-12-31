#include <gtest/gtest.h>
#include "Request.hpp"
#include "Router.hpp"
#include "GetHandler.hpp"
#include "Config.hpp"
#include "HttpMessage.hpp"
#include "RequestException.hpp"
#include "TestEnv.hpp"

namespace
{
class GetHandlerTest : public ::testing::Test
{
protected:
	Request *reqGetHtml;
	Request *reqGetPng;
	Request *reqNotFound;
	const std::string expected = "<title>Lorem ipsum dolor sit amet, consectetur adipiscing elit...</title>はじめましての方ですね";

	// テストデータの作成
	virtual void SetUp()
	{
		reqGetHtml = new Request("GET", "/pages/test.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqGetHtml->setAddr(env->_test_clientSocket).setInfo();
		reqGetPng = new Request("GET", "/assets/logo.png", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqGetPng->setAddr(env->_test_clientSocket).setInfo();
		reqNotFound = new Request("GET", "/pages/none.html", "HTTP/1.1", std::map<std::string, std::string>(), "");
		reqNotFound->setAddr(env->_test_clientSocket).setInfo();
	}
};

// 1. HTMLファイルを正しく取得できるか
TEST_F(GetHandlerTest, getHtmlFile)
{
	// テストデータの検証
	GetHandler handler;
	Response *res = dynamic_cast<Response *>(handler.handleRequest(*reqGetHtml, Config::instance()->getHTTPBlock().getServerContext(std::to_string(TEST_SERVER_PORT), "webserve1")));

	EXPECT_EQ(res->getStatus(), "200");
	EXPECT_EQ(res->getBody(), expected);
	EXPECT_EQ(res->getHeader("Content-Type"), "text/html; charset=utf-8");
	delete res;
}

// 2. PNGファイルを正しく取得できるか
TEST_F(GetHandlerTest, getPngFile)
{
	// テストデータの検証
	GetHandler handler;
	Response *res = dynamic_cast<Response *>(handler.handleRequest(*reqGetPng, Config::instance()->getHTTPBlock().getServerContext(std::to_string(TEST_SERVER_PORT), "webserve1")));

	EXPECT_EQ(res->getStatus(), "200");
	EXPECT_EQ(res->getHeader("Content-Type"), "image/png");
	delete res;
}

// 2. ファイルが存在しない場合、404が返されるか
TEST_F(GetHandlerTest, notRequest)
{
	// インスタンスの生成
	GetHandler handler;
	Response *res = dynamic_cast<Response *>(handler.handleRequest(*reqNotFound, Config::instance()->getHTTPBlock().getServerContext(std::to_string(TEST_SERVER_PORT), "webserve1")));
	// テストデータの検証
	EXPECT_EQ(res->getStatus(), "404");
	delete res;
};

};