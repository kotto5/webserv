#include "Socket.hpp"
#include <algorithm>
#include <iostream>

Socket::Socket(int fd): fd_(fd)
{
    locallen_ = sizeof(localaddr_);
    getsockname(fd_, (sockaddr *)&localaddr_, &locallen_);
}

// Socket::Socket(int fd, const sockaddr *addr, socklen_t len): fd_(fd), locallen_(len)
// {
//     std::memcpy(&localaddr_, &addr, len);
// }


Socket::Socket(int fd, const sockaddr *addr, socklen_t len): fd_(fd)
{
    std::memset(&localaddr_, 0, sizeof(localaddr_));
    std::memcpy(&localaddr_, addr, len);
    locallen_ = len;
}

const sockaddr_in &Socket::getLocaladdr() { return localaddr_; }
socklen_t Socket::getLocallen() { return locallen_; }

Socket::~Socket() {
    close(fd_);
}

int Socket::getFd() { return fd_; }

ClSocket::ClSocket(int fd, sockaddr_in *remoteaddr, socklen_t remotelen): Socket(fd)
{
    std::memset(&remoteaddr_, 0, sizeof(remoteaddr_));
    std::memcpy(&remoteaddr_, remoteaddr, remotelen);
    remotelen_ = remotelen;
}

ClSocket::~ClSocket() {}