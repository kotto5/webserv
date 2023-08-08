#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctime>

class Socket
{
protected:
	int			fd_;
	std::time_t	lastAccess_;
	sockaddr_in	localAddr_;
	socklen_t	localLen_;

public:
	Socket(int fd);
	Socket(int fd, const sockaddr *addr, socklen_t len);
	virtual ~Socket();

	int getFd();
	bool				isTimeout(std::time_t current_time = std::time(NULL));
	const sockaddr_in	&getLocalAddr();
	socklen_t			getLocalLen();
	const std::time_t	&getLastAccess();
	void				updateLastAccess();

	static std::time_t	timeLimit;
};

// =============================================

class ClSocket : public Socket
{
private:
	sockaddr_in	remoteAddr_;
	socklen_t	remoteLen_;

public:
	ClSocket(int fd, sockaddr *remote_addr, socklen_t remote_len);
	ClSocket(int fd, const sockaddr *addr, socklen_t len, sockaddr *remote_addr, socklen_t remote_len);
	~ClSocket();
	const sockaddr_in &getRemoteAddr() { return remoteAddr_; }
	socklen_t getRemoteLen() { return remoteLen_; }
};

// =============================================


class SvSocket : public Socket
{
private:
	static int  createSvSocket(int port);
public:
	SvSocket(int port);
	~SvSocket();
	ClSocket *dequeueSocket();
};

#endif