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

typedef enum E_TYPE {
	TYPE_RECV = 0,
	TYPE_SEND = 1,
	TYPE_SERVER = 2,
	TYPE_CGI = 3,
} T_TYPE;

typedef	std::pair<std::string, std::string>	massages;

class Server {
	private:
		int							strtoi(std::string str);
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
		ssize_t				recv(std::list<int>::iterator itr, std::string &recieving);
		ssize_t				send(std::list<int>::iterator itr, std::string &response);
		int					finish_recv(std::list<int>::iterator itr, std::string &recieving, bool is_cgi_connection);
		int					finish_send(std::list<int>::iterator itr, bool is_cgi_connection);
		bool				request_wants_cgi(Request *request);
		int					new_connect_cgi(Request *request, int client_fd);
		int					setFd(int type, int fd, int client_fd = -1);
		int					eraseFd(int fd, int type);

	static bool			does_finish_recv(const std::string &request, bool is_cgi_connection, ssize_t recv_ret);
	static bool			does_finish_send(const std::string &request, ssize_t recv_ret);
	static int			set_fd_set(fd_set &set, std::list<int> sockets, int &maxFd);
	static Request		*parse_request(const std::string &row_request);
	static std::string	make_response(Request *request);
};

#endif
