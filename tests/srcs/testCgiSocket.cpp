#include <gtest/gtest.h>
#include "TestEnv.hpp"
#include "Socket.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

#define SOCKET_PORT 82

namespace
{
class SocketTest : public ::testing::Test
{
protected:
    int socketFd;
    SvSocket *svSocket;
	virtual void SetUp()
	{
        socketFd = socket(AF_INET, SOCK_STREAM, 0);
        std::cout << "socketFd: " << socketFd << std::endl;
        if (socketFd == -1)
            throw std::runtime_error("socket error");
        svSocket = new SvSocket(SOCKET_PORT);
        if (svSocket == NULL || svSocket->getFd() == -1)
            throw std::runtime_error("svSocket error");
	}
    virtual void TearDown()
    {
        delete svSocket;
        close(socketFd);
    }
};

// 1. リクエストクラスが正しく生成されているか
TEST_F(SocketTest, Socket)
{
    Socket *socket = new Socket(socketFd);
    EXPECT_TRUE(socket != NULL);
    EXPECT_EQ(socket->getFd(), socketFd);
    delete socket;
}

#include <string.h>

TEST_F(SocketTest, ClSocket)
{
    std::cout << sizeof(sockaddr) << std::endl;
    std::cout << sizeof(sockaddr_in) << std::endl;
    std::cout << sizeof(sockaddr_in6) << std::endl;

    int connectFd = socket(AF_INET, SOCK_STREAM, 0);
    if (connectFd == -1)
        throw std::runtime_error("socket error");

    // sockaddr_in addr;
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));

    int ret = connect(connectFd, (struct sockaddr *)&svSocket->getLocalAddr(), svSocket->getLocalLen());
    if (ret)
    {
        perror("connect");
        throw std::runtime_error("connect error");
    }

    if (getsockname(connectFd, (sockaddr *)&addr, &len))
    {
        perror("getsockname");
        throw std::runtime_error("getsockname error");
    }
    uint16_t clientPort = ntohs(addr.sin_port);

    ClSocket *clSock = svSocket->dequeueSocket();
    EXPECT_TRUE(clSock != NULL);
    EXPECT_TRUE(clSock->getFd() != -1);

    EXPECT_EQ(ntohs(clSock->getRemoteAddr().sin_port), clientPort);
}

TEST_F

}