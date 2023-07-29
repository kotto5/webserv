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
	std::time_t	last_access_;
	sockaddr_in	localaddr_;
	socklen_t	locallen_;

public:
	Socket(int fd);
	Socket(int fd, const sockaddr *addr, socklen_t len);
	virtual ~Socket();

	int getFd();
	bool				isTimeout(std::time_t current_time = std::time(NULL));
	const sockaddr_in	&getLocaladdr();
	socklen_t			getLocallen();
	const std::time_t	&getLastAccess();
	void				updateLastAccess();

	static std::time_t	timeLimit;
};

// =============================================

class ClSocket : public Socket
{
private:
	sockaddr_in	remoteaddr_;
	socklen_t	remotelen_;

public:
	ClSocket(int fd, sockaddr *remoteaddr, socklen_t remotelen);
	ClSocket(int fd, const sockaddr *addr, socklen_t len, sockaddr *remoteaddr, socklen_t remotelen);
	~ClSocket();
	const sockaddr_in &getRemoteaddr() { return remoteaddr_; }
	socklen_t getRemotelen() { return remotelen_; }
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