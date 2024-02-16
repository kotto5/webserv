#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <netinet/in.h>
#include <fcntl.h>
#include <map>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include "Request.hpp"
#include <list>
#include "Socket.hpp"
#include "HttpMessage.hpp"
#include "Response.hpp"
#include "Connection.hpp"

#define BUFFER_LEN 10000
#define MAX_SOCKETS 1024
// #define TEST

#define RED "\x1b[41m"
#define BLUE "\x1b[44m"
#define DEF "\x1b[49m"

typedef	std::pair<std::string, std::string>	massages;

class Server
{
public:
	typedef enum E_TYPE {
		E_RECV = 0,
		E_SEND = 1,
		E_SERVER = 2,
	} S_TYPE;
	Server();
	~Server();
	int				setup();
	int				run();

private:
	std::list<Socket *>					server_sockets;
	timeval								timeout;
	std::size_t							_limitClientMsgSize;
	int									_socketCount;
	std::map<sSelectRequest, Connection *>	_connections;

	Socket			*getHandleSock(Socket *sock, HttpMessage *recvdMessage, HttpMessage *toSendMessage);
	int				handleSockets(fd_set *read_fds, fd_set *write_fds, int activity);
	Connection		*accept(Socket *serverSock);
	int				handleConnectionErr(E_TYPE type, std::list<Socket *>::iterator sockNode, bool timeout);
	int				setCgiErrorResponse(CgiSocket *cgiSock, bool timeout);

// utils
	int				createServerSocket(int port) throw();
	static int		set_fd_set(fd_set &set, std::list<Socket *> sockets, int &maxFd) throw();
};
#endif
