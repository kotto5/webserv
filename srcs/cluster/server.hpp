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
		int server_socket_;
		std::list<int>	server_sockets;
		std::list<int>	recv_sockets;
		std::list<int>	send_sockets;

		// std::map<int, std::pair<std::string, std::string>> client_sockets;
		// std::map<int, std::pair<std::string, std::string>> client_sockets;
		std::map<int, massages> client_sockets;
		std::map<int, std::string> requests;
		std::map<int, std::string> responses;

	public:
		Server();
		~Server();
		int	get_server_socket();
		int	create_server_socket();
		int	handle_new_connection(int listen_socket);
		int	run();
		// int	recieve(int &activity, fd_set &read_fds);
		int	recieve(int &activity, fd_set &read_fds, int (&socket_recv)[MAX_CLIENTS], int (&socket_send)[MAX_CLIENTS]);
		int	sender(int &activity, fd_set &write_fds, int (&socket_send)[MAX_CLIENTS], int &client_count);
		int	recv2(std::list<int>::iterator itr, std::string &request);
		int	send2(std::list<int>::iterator itr, std::string &response);
	// flags と len はおいおい。
	int			handle_sockets(fd_set *read_fds, fd_set *write_fds, fd_set *expect_fds);
	static int	set_fd_set(fd_set &set, std::list<int> sockets, int &maxFd);
	static T_STATUS recv(int socket_fd, std::string &request);
	static T_STATUS send(int socket_fd, std::string &response);
	static Request	*make_request(const std::string &row_request);
};



#endif

// 