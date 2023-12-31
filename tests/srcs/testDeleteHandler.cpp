#include <gtest/gtest.h>
#include "Request.hpp"
#include "Config.hpp"
#include "DeleteHandler.hpp"
#include "RequestException.hpp"
#include <fstream>
#include <streambuf>
#include "TestEnv.hpp"
#include "ServerContext.hpp"

namespace
{
class DeleteHandlerTest : public ::testing::Test
{
protected:
	const std::string method = "DELETE";
	const std::string uri = "/resources/unit_test/sample.txt";
	const std::string protocol = "HTTP/1.1";
	std::map<std::string, std::string> headers;
	const std::string body = "";

	// テストデータの作成
	virtual void SetUp()
	{
		headers.insert(std::make_pair("content-length", "100"));
		headers.insert(std::make_pair("content-type", "text/html"));
	}
	virtual void TearDown() {}
};

// =============================================
// 正常系テスト
// =============================================

// 1. リソースが正しく削除されるか（txt）
TEST_F(DeleteHandlerTest, deleteTextFile)
{
	// テストファイルを作成
	std::ofstream ofs("docs/storage/unit_test/sample_delete.txt");
	ofs << "This message is for unit_test.";

	DeleteHandler handler;
	Request req(method, "/resources/unit_test/sample_delete.txt", protocol, headers, body);
	req.setAddr(env->_test_clientSocket).setInfo();
	Response *res = dynamic_cast<Response *>(handler.handleRequest(req, Config::instance()->getHTTPBlock().getServerContext(std::to_string(TEST_SERVER_PORT), "webserve1")));

	//　レスポンスが正しいか
	EXPECT_EQ(res->getStatus(), "204");
	EXPECT_EQ(res->getBody(), "");

	// テストファイルが削除されているか
    std::ifstream file("docs/storage/unit_test/sample_delete.txt");
	EXPECT_FALSE(file.is_open());
}

// =============================================
// 異常系テスト
// =============================================

// 1. 存在しないディレクトリを指定するとエラーになるか
TEST_F(DeleteHandlerTest, deleteFileWithInvalidPath)
{
	DeleteHandler handler;
	Request req(method, "/resources/unit_test/invalid_path/sample.txt", protocol, headers, body);

	// テストデータの検証
	Response *res;
	res = dynamic_cast<Response *>(handler.handleRequest(req, Config::instance()->getHTTPBlock().getServerContext(std::to_string(TEST_SERVER_PORT), "webserve1")));
	EXPECT_EQ(res->getStatus(), "404");
}

// 2. ファイルの削除に失敗した場合にエラーになるか
TEST_F(DeleteHandlerTest, deleteFileFailed)
{
	// テストファイルを作成
	std::ofstream ofs("docs/storage/unit_test/sample_permission.txt");
	ofs << "This message is for unit_test.";

	// テストファイルのパーミッションを変更
	chmod("docs/storage/unit_test/", 0000);

	DeleteHandler handler;
	Request req(method, "/resources/unit_test/sample_permission.txt", protocol, headers, body);

	Response *res;
	res = dynamic_cast<Response *>(handler.handleRequest(req, Config::instance()->getHTTPBlock().getServerContext(std::to_string(TEST_SERVER_PORT), "webserve1")));
	EXPECT_EQ(res->getStatus(), "404");
	// テストファイルのパーミッションを戻す
	chmod("docs/storage/unit_test/", 0777);
}
}
