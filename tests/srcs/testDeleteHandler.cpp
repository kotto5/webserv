#include <gtest/gtest.h>
#include "Request.hpp"
#include "Config.hpp"
#include "DeleteHandler.hpp"
#include <fstream>
#include <streambuf>

namespace
{
class DeleteHandlerTest : public ::testing::Test
{
protected:
	const std::string method = "DELETE";
	const std::string uri = "/resource/unit_test/sample.txt";
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
	std::ofstream ofs("docs/resource/unit_test/sample_delete.txt");
	ofs << "This message is for unit_test.";

	DeleteHandler handler;
	Request req(method, "/resource/unit_test/sample_delete.txt", protocol, headers, body);
	Response res = handler.handleRequest(req);

	//　レスポンスが正しいか
	EXPECT_EQ(res.getStatus(), 204);
	EXPECT_EQ(res.getBody(), "");

	// テストファイルが削除されているか
    std::ifstream file("docs/resource/unit_test/sample_delete.txt");
	EXPECT_FALSE(file.is_open());
}

// =============================================
// 異常系テスト
// =============================================

// 1. 存在しないディレクトリを指定するとエラーになるか
TEST_F(DeleteHandlerTest, deleteFileWithInvalidPath)
{
	DeleteHandler handler;
	Request req(method, "/resource/unit_test/invalid_path/sample.txt", protocol, headers, body);
	Response res = handler.handleRequest(req);

	// テストデータの検証
	EXPECT_EQ(res.getStatus(), 404);
	EXPECT_EQ(res.getBody(), "");
}

// 2. ファイルの削除に失敗した場合にエラーになるか
TEST_F(DeleteHandlerTest, deleteFileFailed)
{
	// テストファイルを作成
	std::ofstream ofs("docs/resource/unit_test/sample_permission.txt");
	ofs << "This message is for unit_test.";

	// テストファイルのパーミッションを変更
	chmod("docs/resource/unit_test/", 0000);

	DeleteHandler handler;
	Request req(method, "/resource/unit_test/sample_permission.txt", protocol, headers, body);
	Response res = handler.handleRequest(req);

	// テストデータの検証
	EXPECT_EQ(res.getStatus(), 404);
	EXPECT_EQ(res.getBody(), "");

	// テストファイルのパーミッションを戻す
	chmod("docs/resource/unit_test/", 0777);
}
}
