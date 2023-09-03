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

#define BUFFER_LEN 10000
#define MAX_CLIENTS 1024

#define RED "\x1b[41m"
#define BLUE "\x1b[44m"
#define DEF "\x1b[49m"

typedef enum e_type {
	TYPE_RECV = 0,
	TYPE_SEND = 1,
	TYPE_SERVER = 2,
	TYPE_CGI = 3,
} T_TYPE;

typedef	std::pair<std::string, std::string>	massages;

class Server
{
public:
	Server();
	~Server();
	int				setup();
	int				run();
	void			createSocketForCgi(int type, int fd, const std::string &body, Socket *sock = NULL);
	int				addRecv(Socket *sock, HttpMessage *message);
	int				addSend(Socket *sock, HttpMessage *message);
	int				addCgi(Socket *sock, Socket *cgi);

private:
	std::list<Socket *>					server_sockets;
	std::list<Socket *>					recv_sockets;
	std::list<Socket *>					send_sockets;
	std::map<Socket *, HttpMessage *>	Recvs;
	std::map<Socket *, HttpMessage *>	Sends;
	timeval								timeout;
	std::size_t							_limitClientMsgSize;

	int				createServerSocket(int port);
	int				handleSockets(fd_set *read_fds, fd_set *write_fds, int activity);
	int				accept(Socket *serverSocket);
	int				recv(Socket *sock, HttpMessage *message);
	ssize_t			send(Socket *sock, HttpMessage *message);
	void 			finishRecv(Socket *sock, HttpMessage *message);
	void			finishSend(Socket *sock, HttpMessage *message);
	void			recvError(Socket *sock);
	int				setFd(int type, Socket *sock);
	int				deleteSocket(int type, Socket *socket);
	bool			checkTimeout();
	static int		set_fd_set(fd_set &set, std::list<Socket *> sockets, int &maxFd);
	void			addKeepAliveHeader(Response *response, ClSocket *clientSock, HttpMessage *request);
	int				setErrorResponse(Socket *clSock);
	void			ErrorfinishSendCgi(CgiSocket *cgiSock, Socket *clSock);
};

#endif
