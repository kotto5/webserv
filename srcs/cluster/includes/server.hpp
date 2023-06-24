#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

typedef enum E_STATUS {
    RECV_ERROR = -1,
    RECV_CONTINUE = 0,
    RECV_FINISHED = 1
} T_STATUS;

class Server {
    private:
        int server_socket_;
    public:
        Server();
        ~Server();
    // flags と len はおいおい。
    T_STATUS recv(int socket_fd, std::string &request);
};

Server::Server() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int yes = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }
    if (bind(server_socket_, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("ERROR on binding");
        exit(1);
    }
    if (listen(server_socket_, 100) < 0){
        perror("ERROR on listening");
        exit(1);
    }

    set_non_blocking(server_socket_);
}

Server::~Server() {}

#include <sys/socket.h>

#define BUFFER_LEN 1024

bool    does_finish(std::string &request){
    (void)request;
    return (true);
}

T_STATUS recv(int socket_fd, std::string &request) {
    ssize_t recv_ret;
    static char buffer[BUFFER_LEN];

    recv_ret = recv(socket_fd, buffer, BUFFER_LEN, 0);
    if (recv_ret == -1)
        return (RECV_ERROR);
    request += buffer;
    if (does_finish(request) == false)
        return (RECV_CONTINUE);
    return (RECV_FINISHED);
}

#endif
