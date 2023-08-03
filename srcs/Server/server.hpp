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

#define BUFFER_LEN 1024
#define MAX_CLIENTS 1024

#define RED "\x1b[41m"
#define DEF "\x1b[49m"

typedef enum e_type {
	TYPE_RECV = 0,
	TYPE_SEND = 1,
	TYPE_SERVER = 2,
	TYPE_CGI = 3,
} T_TYPE;

typedef	std::pair<std::string, std::string>	massages;

class Server {
	private:
		int								strtoi(std::string str);
		std::list<Socket *>				server_sockets;
		std::list<Socket *>				recv_sockets;
		std::list<Socket *>				send_sockets;
		std::map<Socket *, HttpMessage *>	Recvs;
		std::map<Socket *, HttpMessage *>	Sends;
		std::map<Socket *, Socket *>	cgi_client;
		timeval							timeout;

	public:
		Server();
		~Server();
		int					setup();
		int					create_server_socket(int port);
		int					run();
		int					handle_sockets(fd_set *read_fds, fd_set *write_fds, fd_set *expect_fds, int &activity);
		int					accept(Socket *serverSocket);
		ssize_t				recv(Socket *sock, HttpMessage *message);
		ssize_t				send(Socket *sock, HttpMessage *message);
		int					finish_recv(std::list<Socket *>::iterator itr, HttpMessage *request, bool is_cgi_connection);
		int					finish_send(std::list<Socket *>::iterator itr, HttpMessage *response, bool is_cgi_connection);
		bool				request_wants_cgi(Request *request);
		int					new_connect_cgi(Request *request, Socket *clientSocket);
		int					setFd(int type, Socket *sock, Socket *client_sock = NULL);
		int					eraseFd(Socket *socket, int type);
		bool				check_timeout();

	static bool			does_finish_recv(const std::string &request, bool is_cgi_connection, ssize_t recv_ret);
	static bool			does_finish_send(const std::string &request, ssize_t recv_ret);
	static int			set_fd_set(fd_set &set, std::list<Socket *> sockets, int &maxFd);
	static Request		*parse_request(const std::string &row_request);
	static Response		*makeResponse(Request *request);
};

#endif
