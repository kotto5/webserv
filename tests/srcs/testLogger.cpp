#include <gtest/gtest.h>
#include "Logger.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "ErrorCode.hpp"

namespace
{
class LoggerTest : public ::testing::Test
{
protected:
	const std::string accessLogfilePath = "./logs/ut_access.log";
	const std::string errorLogfilePath = "./logs/ut_error.log";

	const std::string method = "GET";
	const std::string url = "/index.html";
	const std::string protocol = "HTTP/1.1";
	std::map<std::string, std::string> headers;
	const std::string body = "Hello World";


	virtual void SetUp()
	{
		// ログファイルを一旦クリア
		std::ofstream accessStream(accessLogfilePath, std::ofstream::out | std::ofstream::trunc);
		accessStream.close();
		std::ofstream errorStream(errorLogfilePath, std::ofstream::out | std::ofstream::trunc);
		errorStream.close();

		// テストデータの挿入
		headers.insert(std::make_pair("content-length", "100"));
		headers.insert(std::make_pair("content-type", "text/html"));
	}

	virtual void TearDown()
	{
	}
};

// 1.インスタンスが正しく生成されるか
TEST_F(LoggerTest, LoggerSetup)
{
	Logger *logger = Logger::instance();
	EXPECT_TRUE(logger != NULL);
}

// 2.インスタンスが取得でき、かつシングルトンであるか
TEST_F(LoggerTest, getInstance)
{
	Logger *logger = Logger::instance();
	EXPECT_TRUE(logger != NULL);

	// 両者は同一のアドレスを持つ
	Logger *logger2 = Logger::instance();
	EXPECT_TRUE(logger == logger2);
}

// 3.ログファイルのパスを参照できるか
TEST_F(LoggerTest, getLogfilePath)
{
	const std::ofstream &ofs1 = Logger::instance()->getAccessLogStream();;
	const std::ofstream &ofs2 = Logger::instance()->getErrorLogStream();

	EXPECT_TRUE(ofs1.is_open());
	EXPECT_TRUE(ofs2.is_open());
}

// 4. アクセスログを書き込めるか
TEST_F(LoggerTest, writeAccessLog)
{
	// テストデータの挿入
	const Request req = Request(method, url, protocol, headers, body);
	const Response res = Response("200", headers, body);

	// アクセスログへの書き込み
	Logger::instance()->writeAccessLog(req, res);
	// ファイルの読み込み
	std::ifstream ifs(accessLogfilePath);
	if (!ifs.is_open())
	{
		std::cout << "file not open" << std::endl;
	}
	sleep(1);
	std::string line;
	std::getline(ifs, line);

	// テストデータの検証
	EXPECT_TRUE(line.find("GET") != std::string::npos);

	// ファイルを閉じる
	ifs.close();
}

// 5. エラーログを書き込めるか
TEST_F(LoggerTest, writeErrorLog)
{
	// テストデータの挿入
	const Request req = Request(method, url, protocol, headers, body);
	const Response res = Response("200", headers, body);

	// エラーログへの書き込み
	Logger::instance()->writeErrorLog(ErrorCode::REQ_PARSE, "ErrorMessage", &req);

	// ファイルの読み込み
	std::ifstream ifs(errorLogfilePath);
	std::string line;
	std::getline(ifs, line);

	// テストデータの検証
	EXPECT_TRUE(line.find("Error: Request parse failed.") != std::string::npos);
	EXPECT_TRUE(line.find("ErrorMessage") != std::string::npos);
	EXPECT_TRUE(line.find("HTTP/1.1") != std::string::npos);

	// ファイルを閉じる
	ifs.close();
}
}