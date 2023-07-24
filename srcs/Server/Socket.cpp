#include "Socket.hpp"
#include <algorithm>
#include <iostream>
#include <arpa/inet.h>

#include "Request.hpp"
#include "Error.hpp"
#include "Config.hpp"
#include "LocationContext.hpp"
#include "ServerContext.hpp"
#include "Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Socket::Socket(int fd): fd_(fd)
{
    locallen_ = sizeof(localaddr_);
    getsockname(fd_, (sockaddr *)&localaddr_, &locallen_);

    std::cout << "!!localaddr_.sin_addr.s_addr: " << inet_ntoa(localaddr_.sin_addr) << std::endl;
    std::cout << "!!localaddr_.sin_port: " << ntohs(localaddr_.sin_port) << std::endl;
    std::cout << "==============" << std::endl;
}

// Socket::Socket(int fd, const sockaddr *addr, socklen_t len): fd_(fd), locallen_(len)
// {
//     std::memcpy(&localaddr_, &addr, len);
// }


Socket::Socket(int fd, const sockaddr *addr, socklen_t len): fd_(fd)
{
    // std::memset(&localaddr_, 0, sizeof(localaddr_));
    // std::memcpy(&localaddr_, addr, len);

    localaddr_ = *(sockaddr_in *)addr;
    locallen_ = len;
    std::cout << "localaddr_.sin_addr.s_addr: " << inet_ntoa(localaddr_.sin_addr) << std::endl;
    // std::cout << "localaddr_.sin_addr.s_addr: " << localaddr_.sin_addr << std::endl;
    std::cout << "localaddr_.sin_port: " << ntohs(localaddr_.sin_port) << std::endl;

    sockaddr_in tmp;
    getsockname(fd_, (sockaddr *)&tmp, &len);
    std::cout << "tmp.sin_addr.s_addr: " << inet_ntoa(tmp.sin_addr) << std::endl;
    std::cout << "tmp.sin_port: " << ntohs(tmp.sin_port) << std::endl;
    std::cout << "==============" << std::endl;
}

const sockaddr_in &Socket::getLocaladdr() { return localaddr_; }
socklen_t Socket::getLocallen() { return locallen_; }

Socket::~Socket() {
    close(fd_);
}

int Socket::getFd() { return fd_; }

ClSocket::ClSocket(int fd, sockaddr_in *remoteaddr, socklen_t remotelen): Socket(fd)
{
    // std::memset(&remoteaddr_, 0, sizeof(remoteaddr_));
    // std::memcpy(&remoteaddr_, remoteaddr, remotelen);
    remoteaddr_ = *remoteaddr;
    remotelen_ = remotelen;

    std::cout << "remoteaddr_.sin_addr.s_addr: " << inet_ntoa(remoteaddr_.sin_addr) << std::endl;
    std::cout << "remoteaddr_.sin_port: " << ntohs(remoteaddr_.sin_port) << std::endl;
}

ClSocket::~ClSocket() {}