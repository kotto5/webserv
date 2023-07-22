#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Socket
{
private:
    int fd_;
    sockaddr_in localaddr_;
    socklen_t locallen_;

public:
    Socket(int fd);
    Socket(int fd, const sockaddr *addr, socklen_t len);
    virtual ~Socket();
    int getFd();
    const sockaddr_in &getLocaladdr();
    socklen_t getLocallen();
};

// Socket::Socket(int fd, sockaddr *addr, socklen_t len): fd_(fd), addr_(*addr), len_(len) 
// {}

class ClSocket : public Socket
{
private:
    sockaddr_in remoteaddr_;
    socklen_t remotelen_;

public:
    ClSocket(int fd, sockaddr_in *remoteaddr, socklen_t remotelen);
    ~ClSocket();
    const sockaddr_in &getRemoteaddr() { return remoteaddr_; }
    socklen_t getRemotelen() { return remotelen_; }
};

#endif