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

#define BUFFER_LEN 1024
#define MAX_CLIENTS 1024

#define RED "\x1b[41m"
#define DEF "\x1b[49m"

typedef enum E_STATUS {
	RECV_CONTINUE = 0,
	RECV_ERROR = -1,
	RECV_FINISHED = 1
} T_STATUS;

typedef enum E_TYPE {
	TYPE_RECV = 0,
	TYPE_SEND = 1,
	TYPE_CGI = 2
} T_TYPE;

typedef	std::pair<std::string, std::string>	massages;

class Server {
	private:
		std::list<int>				server_sockets;
		std::list<int>				recv_sockets;
		std::list<int>				send_sockets;
		std::map<int, std::string>	Recvs;
		std::map<int, std::string>	Sends;
		std::map<int, int>			cgi_client;

	public:
		Server();
		~Server();
		int				setup();
		int				create_server_socket(int port);
		int				run();
		int				handle_sockets(fd_set *read_fds, fd_set *write_fds, fd_set *expect_fds, int &activity);
		int				accept(int listen_socket);
		int				setFd(int fd, int type);
		int				setcgiFd(int fd, int client_fd);
		int				recv(std::list<int>::iterator itr, std::string &request);
		int				recv_cgi(std::list<int>::iterator itr, std::string &request);
		int				send(std::list<int>::iterator itr, std::string &response);
		int				send_cgi(std::list<int>::iterator itr, std::string &response);
		static int		set_fd_set(fd_set &set, std::list<int> sockets, int &maxFd);
		static Request	*make_request(const std::string &row_request);
		static std::string	make_response(Request *request);
		static bool		does_finish_recv_request(const std::string &request);
};

#endif
