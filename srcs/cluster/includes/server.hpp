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

#define BUFFER_LEN 1024
#define MAX_CLIENTS 1024

typedef enum E_STATUS {
    RECV_ERROR = -1,
    RECV_CONTINUE = 0,
    RECV_FINISHED = 1
} T_STATUS;

typedef	std::pair<std::string, std::string>	massages;

class Server {
    private:
        int server_socket_;
		// std::map<int, std::pair<std::string, std::string>> client_sockets;
		// std::map<int, std::pair<std::string, std::string>> client_sockets;
		std::map<int, massages> client_sockets;

    public:
        Server();
        ~Server();
		int	get_server_socket();
		int	handle_new_connection();
		int	recieve(int &activity, fd_set &read_fds);
    // flags と len はおいおい。
    static T_STATUS recv(int socket_fd, std::string &request);

};



#endif

// 