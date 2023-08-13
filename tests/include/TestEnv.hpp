#pragma once

#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "Config.hpp"
#include "Logger.hpp"
#include "Socket.hpp"

class TestEnv: public testing::Environment{
public:
	virtual ~TestEnv() {}
	ClSocket *socket = NULL;
	struct sockaddr_in address;

	virtual void SetUp()
	{
		// 設定ファイルの読み込み
		Config::initialize("./conf/testConfig/defaultTest.conf");
		Logger::initialize("./logs/ut_access.log", "./logs/ut_error.log");

		// テスト用ディレクトリを作成
		std::string command = "./tests/scripts/clean.sh";
		std::string path = "docs/storage/unit_test";
		std::system((command + " " + path).c_str());

		// クライアントソケットをモック化
		std::memset(&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
		address.sin_port = htons(80);

		socket = new ClSocket(42, (struct sockaddr *)&address, sizeof(address), (struct sockaddr *)&address, sizeof(address));
	}
	virtual void TearDown() {}
};

extern TestEnv *env;