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

typedef	std::pair<std::string, std::string>	massages;

class Server {
	private:
		std::list<int>				server_sockets;
		std::list<int>				recv_sockets;
		std::list<int>				send_sockets;
		std::map<int, std::string>	requests;
		std::map<int, std::string>	responses;

	public:
		Server();
		~Server();
		int				setup();
		int				create_server_socket();
		int				run();
		int				handle_sockets(fd_set *read_fds, fd_set *write_fds, fd_set *expect_fds, int &activity);
		int				accept(int listen_socket);
		int				recv(std::list<int>::iterator itr, std::string &request);
		int				send(std::list<int>::iterator itr, std::string &response);
		static int		set_fd_set(fd_set &set, std::list<int> sockets, int &maxFd);
		static Request	*make_request(const std::string &row_request);
		static std::string		make_response(std::string request_raw);
		static bool		does_finish_recv_request(const std::string &request);
};

#endif
