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

Socket::Socket(int fd): fd_(fd), last_access_(std::time(NULL))
{
    locallen_ = sizeof(localaddr_);
    getsockname(fd_, (sockaddr *)&localaddr_, &locallen_);
}

Socket::Socket(int fd, const sockaddr *addr, socklen_t len): fd_(fd), last_access_(std::time(NULL)),
    localaddr_(*(sockaddr_in *)addr), locallen_(len){
}

Socket::~Socket() {
    close(fd_);
}

const sockaddr_in		&Socket::getLocaladdr() { return localaddr_; }
socklen_t				Socket::getLocallen() { return locallen_; }
const std::time_t		&Socket::getLastAccess() { return last_access_; }
void					Socket::updateLastAccess() { last_access_ = std::time(NULL); }
bool					Socket::isTimeout(std::time_t current_time) { return (current_time - last_access_ > timeLimit); }

int Socket::getFd() { return fd_; }

// =============================================
// ============ ClSocket class ===================
// =============================================

ClSocket::ClSocket(int fd, const sockaddr *addr, socklen_t len, sockaddr *remoteaddr, socklen_t remotelen):
    Socket(fd, addr, len), remoteaddr_(*(sockaddr_in *)remoteaddr), remotelen_(remotelen) {}

ClSocket::ClSocket(int fd, sockaddr *remoteaddr, socklen_t remotelen): Socket(fd)
{
    remoteaddr_ = *(sockaddr_in *)(remoteaddr);
    remotelen_ = remotelen;
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
    struct  sockaddr_in remoteaddr;
    socklen_t remotelen = sizeof(remoteaddr);

    int clientFd = accept(this->fd_, (sockaddr *)&remoteaddr, &remotelen);
    if (clientFd == -1)
        return NULL;
	set_non_blocking(clientFd);
    return (new ClSocket(clientFd, (sockaddr *)&this->localaddr_, this->locallen_, (sockaddr *)&remoteaddr, remotelen));
}
