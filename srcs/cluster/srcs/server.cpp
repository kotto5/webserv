#include "server.hpp"
#include "utils.hpp"

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

bool    does_finish(const std::string &request){
	size_t	end_of_header;
	end_of_header = request.find("\r\n\r\n");
	if (end_of_header == std::string::npos)
		return (false);
    if (request.find("content-length: ") != std::string::npos && 
			find_start(request, end_of_header, "\r\n\r\n") == std::string::npos)
        return (false);
	return (true);
}

T_STATUS Server::recv(int socket_fd, std::string &request) {
    ssize_t recv_ret;
    static char buffer[BUFFER_LEN];

    recv_ret = ::recv(socket_fd, buffer, BUFFER_LEN, 0);
    if (recv_ret == -1)
        return (RECV_ERROR);
    request += buffer;
    if (does_finish(request) == false)
        return (RECV_CONTINUE);
    return (RECV_FINISHED);
}

int	Server::handle_new_connection(){
    struct sockaddr_in		client_address;
    socklen_t				client_length = sizeof(client_address);
    int 					new_socket = accept(server_socket_, (struct sockaddr*) &client_address, &client_length);

    if (new_socket < 0) {
        perror("ERROR on accept");
        exit(1);
    }
    set_non_blocking(new_socket);
	// client_sockets.emplace(new_socket, );
	std::pair<std::string, std::string> tmp;
	client_sockets[new_socket] = tmp;

    // std::cout << "New connection, socket fd is " << new_socket << ", port is " << ntohs(client_address.sin_port) << "index is : " << i << std::endl;
	return (new_socket);
}

int	Server::recieve(int &activity, fd_set &read_fds){
	std::map<int, massages>::iterator itr;
	int		fd;

	for (itr = client_sockets.begin(); itr != client_sockets.end() && activity; itr++) {
		fd = itr->first;
		if (FD_ISSET(fd, &read_fds)) {
			Server::recv(fd, itr->second.first);
			// makeresponse?
			// send()
			activity--;
		}
	}
	return (0);
}

int	Server::get_server_socket(){
	return (this->server_socket_);
}
