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
	Socket(const Socket &other);
	virtual ~Socket();
	Socket &operator=(const Socket &other);

	int getFd();
	bool				isTimeout(std::time_t current_time = std::time(NULL));
	const sockaddr_in	&getLocalAddr() const ;
	socklen_t			getLocalLen() const ;
	int					getLocalPort() const { return ntohs(localAddr_.sin_port); }
	const std::time_t	&getLastAccess() const ;
	void				updateLastAccess();

	static std::time_t	timeLimit;
};

// =============================================

#define MAX_REQUEST 3

class ClSocket : public Socket
{
private:
	sockaddr_in	remoteAddr_;
	socklen_t	remoteLen_;
	int			maxRequest_;

public:
	ClSocket(int fd, sockaddr *remote_addr, socklen_t remote_len);
	ClSocket(int fd, const sockaddr *addr, socklen_t len, sockaddr *remote_addr, socklen_t remote_len);
	ClSocket(const ClSocket &other);
	~ClSocket();
	ClSocket &operator=(const ClSocket &other);

	const sockaddr_in	&getRemoteAddr() const { return remoteAddr_; }
	socklen_t			getRemoteLen() const { return remoteLen_; }
	int					getMaxRequest() const { return maxRequest_; }
	void				decrementMaxRequest() { maxRequest_--; }
	void				setMaxRequest(int maxRequest) { maxRequest_ = maxRequest; }
};

// =============================================


class SvSocket : public Socket
{
private:
	static int  createSvSocket(uint16_t port);
public:
	SvSocket(int port);
	~SvSocket();
	ClSocket *dequeueSocket();
};

// =============================================


class CgiSocket : public Socket
{
private:
	int			pid_;
	ClSocket	*clSocket_;
public:
	CgiSocket(int fd, int pid, ClSocket *clSocket);
	CgiSocket(int fd, ClSocket *clSocket);
	~CgiSocket();
	ClSocket 	*moveClSocket();
	ClSocket	*getClSocket() const;
	int			getPid() const;
	int			killCgi() const;
};

#endif