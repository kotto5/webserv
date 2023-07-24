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
}

Socket::Socket(int fd, const sockaddr *addr, socklen_t len): fd_(fd),
    localaddr_(*(sockaddr_in *)addr), locallen_(len) {}

const sockaddr_in &Socket::getLocaladdr() { return localaddr_; }
socklen_t Socket::getLocallen() { return locallen_; }

Socket::~Socket() {
    close(fd_);
}

int Socket::getFd() { return fd_; }

ClSocket::ClSocket(int fd, sockaddr_in *remoteaddr, socklen_t remotelen): Socket(fd)
{
    remoteaddr_ = *remoteaddr;
    remotelen_ = remotelen;
}

ClSocket::~ClSocket() {}