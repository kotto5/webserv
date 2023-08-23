#include <gtest/gtest.h>
#include "Request.hpp"
#include "Config.hpp"
#include "PostHandler.hpp"
#include <fstream>
#include <streambuf>
#include "HttpMessage.hpp"
#include "RequestException.hpp"
#include "TestEnv.hpp"

namespace
{
class PostHandlerTest : public ::testing::Test
{
protected:
	const std::string method = "POST";
	const std::string uri = "/resources/unit_test/sample.html";
	const std::string protocol = "HTTP/1.1";
	std::map<std::string, std::string> headers;
	const std::string body = "This message is for unit_test.";

	// テストデータの作成
	virtual void SetUp()
	{
		headers.insert(std::make_pair("content-length", "100"));
		headers.insert(std::make_pair("content-type", "text/html"));
	}
	virtual void TearDown() {}
};

// ファイルをバイナリで読み込む
std::string readFile(const std::string &filePath)
{
	std::ifstream file(filePath, std::ios::binary);
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

// =============================================
// 正常系テスト
// =============================================

// 1. リソースが正しく作成されるか（html）
TEST_F(PostHandlerTest, createTextFile)
{
	// インスタンスの生成
	PostHandler handler;
	std::string body = readFile("./docs/pages/test.html");
	Request req(method, "/resources/unit_test/sample.html", protocol, headers, body);
	req.setAddr(env->socket).setInfo();
	Response *res = handler.handleRequest(req);

	std::string file_data = readFile("./docs/storage/unit_test/sample.html");

	// テストデータの検証
	EXPECT_EQ(res->getStatus(), "201");
	EXPECT_EQ(file_data, body);
	EXPECT_EQ(res->getBody(), "");
	EXPECT_EQ(res->getHeader("Location"), "/resources/unit_test/sample.html");
	EXPECT_EQ(res->getHeader("Content-Type"), "text/html; charset=utf-8");
}

// 2. リソースが正しく作成されるか（png）
TEST_F(PostHandlerTest, createPngFile)
{
	PostHandler handler;
	std::string body = readFile("./docs/test.png");
	Request req(method, "/resources/unit_test/sample.png", protocol, headers, body);
	req.setAddr(env->socket).setInfo();
	Response *res = handler.handleRequest(req);

	std::string file_data = readFile("./docs/storage/unit_test/sample_post.png");

	// テストデータの検証
	EXPECT_EQ(res->getStatus(), "201");
	EXPECT_EQ(file_data, body);
	EXPECT_EQ(res->getBody(), "");
	EXPECT_EQ(res->getHeader("Location"), "/resources/unit_test/sample.png");
	EXPECT_EQ(res->getHeader("Content-Type"), "image/png");
}

// 3. リソースが複数回正しく作成されるか
TEST_F(PostHandlerTest, createMultiFile)
{
	PostHandler handler;
	std::string body = readFile("./docs/test.txt");
	Request req(method, "/resources/unit_test/sample_multi.txt", protocol, headers, body);
	req.setAddr(env->socket).setInfo();
	Response *res_0 = handler.handleRequest(req); // 1回目
	Response *res_1 = handler.handleRequest(req); // 2回目
	Response *res_2 = handler.handleRequest(req); // 3回目

	std::string file_data_0 = readFile("./docs/storage/unit_test/sample_multi.txt");
	std::string file_data_1 = readFile("./docs/storage/unit_test/0sample_multi.txt");
	std::string file_data_2 = readFile("./docs/storage/unit_test/1sample_multi.txt");

	// テストデータの検証
	EXPECT_EQ(res_0->getStatus(), "201");
	EXPECT_EQ(file_data_0, body);
	EXPECT_EQ(res_0->getBody(), "");
	EXPECT_EQ(res_0->getHeader("Location"), "/resources/unit_test/sample_multi.txt");
	EXPECT_EQ(res_0->getHeader("Content-Type"), "text/plain; charset=utf-8");

	EXPECT_EQ(res_1->getStatus(), "201");
	EXPECT_EQ(file_data_1, body);
	EXPECT_EQ(res_1->getBody(), "");
	EXPECT_EQ(res_1->getHeader("Location"), "/resources/unit_test/0sample_multi.txt");
	EXPECT_EQ(res_1->getHeader("Content-Type"), "text/plain; charset=utf-8");

	EXPECT_EQ(res_2->getStatus(), "201");
	EXPECT_EQ(file_data_2, body);
	EXPECT_EQ(res_2->getBody(), "");
	EXPECT_EQ(res_2->getHeader("Location"), "/resources/unit_test/1sample_multi.txt");
	EXPECT_EQ(res_2->getHeader("Content-Type"), "text/plain; charset=utf-8");

	delete res_0;
	delete res_1;
	delete res_2;
}

// =============================================
// 異常系テスト
// =============================================

// 1. 存在しないディレクトリを指定するとエラーになるか
TEST_F(PostHandlerTest, createFileWithInvalidPath)
{
	PostHandler handler;
	Request req(method, "/resources/unit_test/invalid_path/sample.txt", protocol, headers, body);
	req.setAddr(env->socket).setInfo();

	// テストデータの検証
	try
	{
		handler.handleRequest(req);
		FAIL() << "Expected RequestException";
	}
	catch (const RequestException &e)
	{
		EXPECT_EQ(e.getStatus(), "404");
	}
	catch (...)
	{
		FAIL() << "Expected specific exception type";
	}

}

TEST_F(PostHandlerTest, createFileFailed)
{
	// ディレクトリのパーミッションを変更
	chmod("docs/storage/unit_test/", 0000);

	PostHandler handler;
	Request req(method, "/resources/unit_test/sample.txt", protocol, headers, body);
	req.setAddr(env->socket).setInfo();

	// テストデータの検証
	try
	{
		handler.handleRequest(req);
		FAIL() << "Expected RequestException";
	}
	catch (const RequestException &e)
	{
		EXPECT_EQ(e.getStatus(), "403");
	}
	catch (...)
	{
		FAIL() << "Expected specific exception type";
	}

	// テストファイルのパーミッションを戻す
	chmod("docs/storage/unit_test/", 0777);
}
}
