#include <gtest/gtest.h>
#include "Logger.hpp"
#include "Response.hpp"
#include "Request.hpp"

namespace
{
class LoggerTest : public ::testing::Test
{
protected:
	Logger *logger;

	const std::string accessLogfilePath = "./logs/access.log";
	const std::string errorLogfilePath = "./logs/error.log";

	const std::string method = "GET";
	const std::string url = "/index.html";
	const std::string protocol = "HTTP/1.1";
	std::map<std::string, std::string> headers;
	const std::string body = "Hello World";

	virtual void SetUp()
	{
		// ログファイルを一旦削除
	//	remove(accessLogfilePath.c_str());
	//	remove(errorLogfilePath.c_str());

		// Loggerインスタンスを生成
		logger = new Logger(
			AccessLogfilePath,
			ErrorLogfilePath
		);

		// テストデータの挿入
		headers.insert(std::make_pair("content-length", "100"));
		headers.insert(std::make_pair("content-type", "text/html"));
	}
};

// 1.インスタンスが正しく生成されるか
TEST_F(LoggerTest, LoggerSetup)
{
	EXPECT_TRUE(logger != NULL);
}

// 2.インスタンスが取得でき、かつシングルトンであるか
TEST_F(LoggerTest, getInstance)
{
	Logger *logger = Logger::getInstance();
	EXPECT_TRUE(logger != NULL);

	// 両者は同一のアドレスを持つ
	Logger *logger2 = Logger::getInstance();
	EXPECT_TRUE(logger == logger2);
}

// 3.ログファイルのパスを参照できるか
TEST_F(LoggerTest, getLogfilePath)
{
	accessLogfilePath = Logger::getInstance()->getAccessLogfilePath();
	errorLogfilePath = Logger::getInstance()->getErrorLogfilePath();

	EXPECT_EQ(accessLogfilePath, "./logs/access.log");
	EXPECT_EQ(errorLogfilePath, "./logs/error.log");
}

// 4. アクセスログを書き込めるか
TEST_F(LoggerTest, writeAccessLog)
{
	// テストデータの挿入
	Request req = Request(method, url, protocol, headers, body);
	Response res = Response(200, headers, body);

	// アクセスログへの書き込み
	Logger::getInstance()->writeAccessLog(req, res);

	// ファイルの読み込み
	std::ifstream ifs(accessLogfilePath);
	std::string line;
	std::getline(ifs, line);

	// テストデータの検証
	EXPECT_TRUE(line.find("GET /index.html HTTP/1.1") != std::string::npos);
	EXPECT_TRUE(line.find("200") != std::string::npos);

	// ファイルを閉じる
	ifs.close();
}
}