#ifndef TESTENV_HPP
#define TESTENV_HPP

#pragma once

#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "Config.hpp"
#include "Logger.hpp"
#include "Socket.hpp"
#include <string.h>
#include "ServerContext.hpp"

#define TEST_SERVER_PORT 1011

class TestEnv: public testing::Environment{
public:
	virtual ~TestEnv() {}
	SvSocket *_test_svSocket = NULL;
	ClSocket *_test_clientSocket = NULL;
	const ServerContext *_test_serverContext = NULL;

	int	_test_clientFd;

	virtual void SetUp()
	{
		// 設定ファイルの読み込み
		Config::initialize("./conf/testConfig/defaultTest.conf");
		Logger::initialize("./logs/ut_access.log", "./logs/ut_error.log");
		_test_serverContext = &Config::instance()->getHTTPBlock().getServerContext(std::to_string(TEST_SERVER_PORT), "webserve1");

		// テスト用ディレクトリを作成
		std::string command = "./tests/scripts/clean.sh";
		std::string path = "docs/storage/unit_test";
		std::system((command + " " + path).c_str());

		// クライアントソケットをモック化
		_test_svSocket = new SvSocket(TEST_SERVER_PORT);
		sockaddr_in address = _test_svSocket->getLocalAddr();
		{
			sockaddr_in *addr = &address;
			socklen_t len = sizeof(*addr);
			int connectFd = socket(AF_INET, SOCK_STREAM, 0);
			if (connectFd == -1)
			{
				throw std::runtime_error("socket error" + std::string(strerror(errno)));
			}
			if (connect(connectFd, (struct sockaddr *)addr, len) == -1)
			{
				throw std::runtime_error("connect error: " + std::string(strerror(errno)));
			}
			_test_clientFd = connectFd;
		}
		_test_clientSocket = _test_svSocket->dequeueSocket();
	}
	virtual void TearDown() {}
};

extern TestEnv *env;

#endif