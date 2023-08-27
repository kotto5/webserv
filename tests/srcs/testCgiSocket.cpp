#include <gtest/gtest.h>
#include "TestEnv.hpp"
#include "Socket.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <errno.h>

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
    int connectFd = socket(AF_INET, SOCK_STREAM, 0);
    if (connectFd == -1)
        throw std::runtime_error("socket error" + std::string(strerror(errno)));
    if (connect(connectFd, (struct sockaddr *)&svSocket->getLocalAddr(), svSocket->getLocalLen()))
        throw std::runtime_error("connect error: " + std::string(strerror(errno)));

    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    if (getsockname(connectFd, (sockaddr *)&addr, &len))
        throw std::runtime_error("getsockname error" + std::string(strerror(errno)));

    ClSocket *clSock = svSocket->dequeueSocket();
    if (clSock == NULL || clSock->getFd() == -1)
        throw std::runtime_error("dequeueSocket error" + std::string(strerror(errno)));

//    struct sockaddr_in {
//       u_char  sin_len;    （このメンバは古いOSでは存在しない）
//       u_char  sin_family;    （アドレスファミリ．今回はAF_INETで固定）
//       u_short sin_port;    （ポート番号）
//       struct  in_addr sin_addr;    （IPアドレス）
//       char    sin_zero[8];    （無視してもよい．「詰め物」のようなもの）
//    };
    sockaddr_in clAddr = clSock->getRemoteAddr();
    EXPECT_EQ(clAddr.sin_addr.s_addr, addr.sin_addr.s_addr);
    EXPECT_EQ(clAddr.sin_len, addr.sin_len);
    EXPECT_EQ(clAddr.sin_family, addr.sin_family);
    EXPECT_EQ(clAddr.sin_port, addr.sin_port);
    EXPECT_EQ(ntohs(clAddr.sin_port), ntohs(addr.sin_port));
    socklen_t clLen = clSock->getRemoteLen();
    EXPECT_EQ(clLen, len);

    sockaddr_in svAddr = clSock->getLocalAddr();
    EXPECT_EQ(svAddr.sin_addr.s_addr, svSocket->getLocalAddr().sin_addr.s_addr);
    EXPECT_EQ(svAddr.sin_len, svSocket->getLocalAddr().sin_len);
    EXPECT_EQ(svAddr.sin_family, svSocket->getLocalAddr().sin_family);
    EXPECT_EQ(svAddr.sin_port, svSocket->getLocalAddr().sin_port);
    EXPECT_EQ(ntohs(svAddr.sin_port), ntohs(svSocket->getLocalAddr().sin_port));
    socklen_t svLen = clSock->getLocalLen();
    EXPECT_EQ(svLen, svSocket->getLocalLen());

    delete clSock;
    close(connectFd);
}


TEST_F(SocketTest, CgiSocket1)
{
    int connectFd = socket(AF_INET, SOCK_STREAM, 0);
    if (connectFd == -1)
        throw std::runtime_error("socket error" + std::string(strerror(errno)));
    if (connect(connectFd, (struct sockaddr *)&svSocket->getLocalAddr(), svSocket->getLocalLen()))
        throw std::runtime_error("connect error: " + std::string(strerror(errno)));

    ClSocket *clSock = svSocket->dequeueSocket();
    if (clSock == NULL || clSock->getFd() == -1)
        throw std::runtime_error("dequeueSocket error" + std::string(strerror(errno)));
    
    int cgiSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (cgiSockFd == -1)
        throw std::runtime_error("socket error" + std::string(strerror(errno)));
    CgiSocket *cgiSock = new CgiSocket(cgiSockFd, clSock);
    EXPECT_EQ(cgiSock->getFd(), cgiSockFd);
    EXPECT_EQ(cgiSock->getClSocket(), clSock);
    delete cgiSock;
    close (connectFd);
}

TEST_F(SocketTest, CgiSocket2)
{
    int connectFd = socket(AF_INET, SOCK_STREAM, 0);
    if (connectFd == -1)
        throw std::runtime_error("socket error" + std::string(strerror(errno)));
    if (connect(connectFd, (struct sockaddr *)&svSocket->getLocalAddr(), svSocket->getLocalLen()))
        throw std::runtime_error("connect error: " + std::string(strerror(errno)));

    ClSocket *clSock = svSocket->dequeueSocket();
    if (clSock == NULL || clSock->getFd() == -1)
        throw std::runtime_error("dequeueSocket error" + std::string(strerror(errno)));
    
    int cgiSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (cgiSockFd == -1)
        throw std::runtime_error("socket error" + std::string(strerror(errno)));
    CgiSocket *cgiSock = new CgiSocket(cgiSockFd, clSock);
    clSock = cgiSock->moveClSocket();
    delete cgiSock;

    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    if (getsockname(connectFd, (sockaddr *)&addr, &len))
        throw std::runtime_error("getsockname error" + std::string(strerror(errno)));
    sockaddr_in clAddr = clSock->getRemoteAddr();
    EXPECT_EQ(clAddr.sin_addr.s_addr, addr.sin_addr.s_addr);
    EXPECT_EQ(clAddr.sin_len, addr.sin_len);
    EXPECT_EQ(clAddr.sin_family, addr.sin_family);
    EXPECT_EQ(clAddr.sin_port, addr.sin_port);
    EXPECT_EQ(ntohs(clAddr.sin_port), ntohs(addr.sin_port));
    socklen_t clLen = clSock->getRemoteLen();
    EXPECT_EQ(clLen, len);

    delete clSock;
    close (connectFd);
}


}