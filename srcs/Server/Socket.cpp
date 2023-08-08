#include "Socket.hpp"
#include <algorithm>
#include <iostream>
#include <arpa/inet.h>

#include "Request.hpp"
#include "ErrorCode.hpp"
#include "Config.hpp"
#include "LocationContext.hpp"
#include "ServerContext.hpp"
#include "Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <utils.hpp>
#include <ctime>
#include "Logger.hpp"
#include "ServerException.hpp"


// =============================================
// ============ Socket class ===================
// =============================================

std::time_t	Socket::timeLimit = 5;

Socket::Socket(int fd): fd_(fd), lastAccess_(std::time(NULL))
{
	std::cout << "Socket::Socket(int fd =" << fd << ")" << std::endl;
    localLen_ = sizeof(localAddr_);
    getsockname(fd_, (sockaddr *)&localAddr_, &localLen_);
}

Socket::Socket(int fd, const sockaddr *addr, socklen_t len): fd_(fd), lastAccess_(std::time(NULL)),
    localAddr_(*(sockaddr_in *)addr), localLen_(len){
	std::cout << "Socket::Socket(fd =" << fd << ")" << std::endl;
}

Socket::~Socket() {
    close(fd_);
}

const sockaddr_in		&Socket::getLocalAddr() { return localAddr_; }
socklen_t				Socket::getLocalLen() { return localLen_; }
const std::time_t		&Socket::getLastAccess() { return lastAccess_; }
void					Socket::updateLastAccess() { lastAccess_ = std::time(NULL); }
bool					Socket::isTimeout(std::time_t current_time) { return (current_time - lastAccess_ > timeLimit); }

int Socket::getFd() { return fd_; }

// =============================================
// ============ ClSocket class ===================ma
// =============================================

ClSocket::ClSocket(int fd, const sockaddr *addr, socklen_t len, sockaddr *remoteAddr, socklen_t remoteLen):
    Socket(fd, addr, len), remoteAddr_(*(sockaddr_in *
	)remoteAddr), remoteLen_(remoteLen) {}

ClSocket::ClSocket(int fd, sockaddr *remoteAddr, socklen_t remoteLen): Socket(fd)
{
    remoteAddr_ = *(sockaddr_in *)(remoteAddr);
    remoteLen_ = remoteLen;
}

ClSocket::~ClSocket() {}

// =============================================
// ============ SvSocket class =================
// =============================================

int SvSocket::createSvSocket(int port)
{
	int	new_sock;
	new_sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int yes = 1;
	if (setsockopt(new_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
        throw ServerException("setsockopt");
	}
	if (bind(new_sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
	{
        throw ServerException("binding");
	}
	if (listen(new_sock, 200) < 0)
	{
        throw ServerException("listening");
	}
	set_non_blocking(new_sock);
    return (new_sock);
}

SvSocket::SvSocket(int port): Socket(createSvSocket(port))
{
}

SvSocket::~SvSocket() {}

ClSocket    *SvSocket::dequeueSocket()
{
    struct  sockaddr_in remote_addr;
    socklen_t remote_len = sizeof(remote_addr);

    int clientFd = accept(this->fd_, (sockaddr *)&remote_addr, &remote_len);
    if (clientFd == -1)
        return NULL;
	set_non_blocking(clientFd);
    return (new ClSocket(clientFd, (sockaddr *)&this->localAddr_, this->localLen_, (sockaddr *)&remote_addr, remote_len));
}
